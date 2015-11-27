/*
 * $Id: tdspp.cc,v 2.7 2005/10/20 11:24:12 martin Exp $
 * Copyright (c) 2004, 2005, Voidsoft AB
 */

#include "tdspp.hh"
#include <iostream>

TDSPP::TDSPP() {
    verbose = 0;
    if (init() != CS_SUCCEED) {
        throw Exception("TDSPP: Init failed");
        return;
    }
}

TDSPP::~TDSPP() {
    disconnect();
    ct_con_drop(conn);
    ct_exit(ctx, CS_UNUSED);
    cs_ctx_drop(ctx);
}

CS_RETCODE TDSPP::init(void) {
    
    CS_RETCODE ret;
    ret = cs_ctx_alloc(CS_VERSION_100, &ctx);
    if (ret != CS_SUCCEED) {
        throw Exception("TDSPP::init: Context allocation failed!");
        return ret;
    }
    ret = ct_init(ctx, CS_VERSION_100);
    if (ret != CS_SUCCEED) {
        throw Exception("TDSPP::init: Library init failed!");
        return ret;
    }
    ret = ct_con_alloc(ctx, &conn);
    if (ret != CS_SUCCEED) {
        throw Exception("TDSPP::init: Connect allocation failed!");
        return ret;
    }
    return CS_SUCCEED;
}

void TDSPP::connect(const string &server, 
                    const string &username, 
                    const string &password) {
    
    this->server = server;
    this->username = username;
    this->password = password;
    
    login();
}

void TDSPP::login(void) {
    if (ct_con_props(conn, CS_SET, CS_USERNAME, 
                    (CS_VOID*)username.c_str(), CS_NULLTERM, 
                    (CS_INT*)NULL) != CS_SUCCEED) {
        throw Exception("TDSPP::login: ct_con_props() SET USERNAME failed!");
        return;
    }
    if (ct_con_props(conn, CS_SET, CS_PASSWORD, 
                     (CS_VOID*)password.c_str(), CS_NULLTERM, 
                     (CS_INT*)NULL) != CS_SUCCEED) {
        throw Exception("TDSPP::login: ct_con_props() SET PASSWORD failed!");
        return;
    }
    if (ct_connect(conn, (CS_CHAR*)server.c_str(), 
                   CS_NULLTERM) != CS_SUCCEED) {
        throw Exception("TDSPP::login: Connection failed!");
        return;
    }
    if (ct_cmd_alloc(conn, &cmd) != CS_SUCCEED) {
        throw Exception("TDSPP::login: Command allocation failed!");
        return;
    }
}

void TDSPP::disconnect(void) {
    if (ct_cancel(conn, NULL, CS_CANCEL_ALL) != CS_SUCCEED) {
        throw Exception("TDSPP::disconnect: ct_cancel() failed!");
        return;
    }
    ct_cmd_drop(cmd);
    ct_close(conn, CS_UNUSED);
}

void TDSPP::execute(const string& sql) {
    
    CS_RETCODE results_ret;
    CS_INT result_type;
    
    if (cmd == NULL) throw Exception("TDSPP::execute: cmd is not allocated");
    
    if (ct_command(cmd, CS_LANG_CMD, 
                  sql.c_str(), CS_NULLTERM, CS_UNUSED) != CS_SUCCEED) {
        throw Exception("TDSPP::execute: ct_command() failed");
        return;
    }
    if (ct_send(cmd) != CS_SUCCEED) {
        throw Exception("TDSPP::execute: ct_send() failed");
        return;
    }
    while ((results_ret = ct_results(cmd, &result_type)) == CS_SUCCEED) {
        switch ((int) result_type) {
        case CS_CMD_SUCCEED:
            break;
        case CS_CMD_DONE:
            break;
        case CS_CMD_FAIL:
            throw Exception(string("TDSPP::execute: Failed on ")+sql);
            // return CS_FAIL;
            break;
        default:
            throw Exception("TDSPP::execute: ct_results() unexpected result_type.");
            return;
        }
    }
    switch ((int) results_ret) {
    case CS_END_RESULTS:
        break;
    case CS_FAIL:
        throw Exception("TDSPP::execute: ct_results() failed.");
        return;
    default:
        throw Exception("TDSPP::execute: ct_results() unexpected return.");
        return;
    }
}

Query* TDSPP::sql(const string& sql) {
    Query* q = new Query(this);
    q->command = sql;
    return q;  
}
