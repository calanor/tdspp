/*
 * $Id: query.cc,v 2.13 2005/10/20 11:24:12 martin Exp $
 * Copyright (c) 2004, 2005, Voidsoft AB
 */

#include "query.hh"

Query::Query(TDSPP* tds)
    : TDS(tds), endoffile(false) {
    rows = new Rows();
}

Query::~Query() {
    delete rows;
}

void Query::init(void) {
}


void Query::execute(void) {
    if (ct_command(TDS->cmd, CS_LANG_CMD, 
                   (CS_CHAR*)command.c_str(), CS_NULLTERM, 
                   CS_UNUSED) != CS_SUCCEED)
        throw TDSPP::Exception("Query::execute: ct_command() failed");
    if (ct_send(TDS->cmd) != CS_SUCCEED) throw TDSPP::Exception("Query::execute: ct_send() failed");
    
    getresults();
    first();
}


void Query::getrc(void) {
    if (ct_res_info(TDS->cmd, CS_ROW_COUNT, 
                    &rowcount, CS_UNUSED, NULL) != CS_SUCCEED)
        throw TDSPP::Exception("Query::getrc: ct_res_info() failed");
}

void Query::addrow(void) {
    Rows::FieldList fl;
    rows->currentrow = rows->rows.size();
    rows->rows.push_back(fl);
    for (int i=0; i < fieldcount; i ++) {
        if (!addfield()) {
            rows->rows.pop_back();
            break;
        }
    }
}

bool Query::addfield(void) {
    CS_DATAFMT datafmt;
    

    if (ct_describe(TDS->cmd, 
                    rows->rows[rows->currentrow].size()+1, &datafmt) 
        != CS_SUCCEED) {
        throw TDSPP::Exception("Query::addfield: ct_describe() failed");
    }

    /** Store enough space for UNICODE chars in tds version >= 7.0 */
    datafmt.maxlength = datafmt.maxlength *2 + 1;

    /** DATETYPE is reported as 8 bytes from server, 
     *  but becomes 32 when converted. Adjusting for that.
     */
    if (datafmt.maxlength < 32) datafmt.maxlength = 32;
    
    //if (datafmt.format == CS_FMT_UNUSED) return false;
    datafmt.format    = CS_FMT_NULLTERM;
    datafmt.datatype  = CS_CHAR_TYPE;
    
    Field* f = new Field(datafmt.name, datafmt.maxlength);
    CS_SMALLINT ind;

    if (ct_bind(TDS->cmd, 
                rows->rows[rows->currentrow].size()+1, 
                &datafmt, 
                f->data, 
                &f->datalength,
                &ind) != CS_SUCCEED) {
        delete f;
        throw TDSPP::Exception("Query::addfield: ct_bind() failed");
    }
    rows->rows[rows->currentrow].push_back(f);
    return true;   
}
   
void Query::getresults(void) {
    
    CS_RETCODE ret;
    CS_RETCODE results_ret;
    CS_INT result_type;
    CS_INT count;
    while ((results_ret = ct_results(TDS->cmd, &result_type)) == CS_SUCCEED) {
        switch ((int) result_type) {
        case CS_CMD_SUCCEED:
            getrc();
            break;
        case CS_CMD_DONE:
            getrc();
            break;
        case CS_CMD_FAIL:
            throw TDSPP::Exception("Query::getresults: ct_results() failed.");
        case CS_ROW_RESULT:
            if (ct_res_info(TDS->cmd, 
                            CS_NUMDATA, 
                            &fieldcount, 
                            CS_UNUSED, 
                            NULL) != CS_SUCCEED)
                throw TDSPP::Exception("Query::getresults: ct_res_info() failed");
            rows->clear();
            do {
                addrow();
                ret = ct_fetch(TDS->cmd, 
                               CS_UNUSED, 
                               CS_UNUSED, 
                               CS_UNUSED, 
                               &count);
            } while (ret == CS_SUCCEED);

            switch ((int) ret) {
            case CS_END_DATA: {
                // Delete unused fields in extra row.
                Rows::FIte i;
                for (i = rows->rows.back().begin(); i != rows->rows.back().end(); ++i) {
                    delete (*i);
                }
                rows->rows.pop_back();
                break;
	    }
            case CS_ROW_FAIL:
                throw TDSPP::Exception("Query::getresults: ct_fetch() returned CS_ROW_FAIL.");
            case CS_FAIL:
                throw TDSPP::Exception("Query::getresults: ct_fetch() returned CS_FAIL.");
            default:
                throw TDSPP::Exception("Query::getresults: ct_fetch() unexpected return.");
            } // switch
            
            rows->currentrow = 0;
            rowcount = rows->rows.size();
            break;
        case CS_COMPUTE_RESULT:
            throw TDSPP::Exception("Query::getresults: ct_results() unexpected CS_COMPUTE_RESULT.");
        default:
            throw TDSPP::Exception("Query::getresults: ct_results() unexpected result_type.");
        } // switch
    } // while
    
    switch ((int) results_ret) {
    case CS_END_RESULTS:
        break;
    case CS_FAIL:
        throw TDSPP::Exception("Query::getresults: ct_results() failed.");
    default:
        throw TDSPP::Exception("Query::getresults: ct_results() unexpected return.");
    } // switch
}

void Query::next(void) {
    rows->currentrow++;
    if (rows->currentrow >= (int)rows->rows.size()) endoffile = true;   
}

Field* Query::fields(int i) {
    return rows->rows[rows->currentrow][i];
}

Field* Query::fields(string s) {
    for (unsigned int i=0; i < rows->rows[rows->currentrow].size(); i++) {
        if (rows->rows[rows->currentrow][i]->colname == s) {
            return rows->rows[rows->currentrow][i];
        }
    }
    return 0;
}

void Query::first(void) {
    rows->currentrow = 0;
    if (rows->rows.size() == 0) {
        endoffile = true;
    } else {
        endoffile = false;
    }
    
}

bool Query::eof() {
    return endoffile;
}

string Query::operator[] (const char *s) {
    return fields(s)->tostr();
}

string Query::operator[] (string s) {
    return fields(s)->tostr();
}
