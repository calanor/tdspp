/*
 * $Id: main.cc,v 1.6 2005/10/20 11:27:42 martin Exp $
 * Copyright (c) 2004, 2005, Voidsoft AB
 */

#include <stdio.h>
#include <string.h>
#include <ctpublic.h>
#include <iostream>
#include <typeinfo>
#include "tdspp.hh"

using namespace std;

int main(int argc, char **argv) {
    
    TDSPP *db = new TDSPP();
    try {
        /* Connect to database. */
        db->connect("localhost:1433", "admin", "12345678");
        /* Execute command. */
        db->execute("use master");
        /* Create query. */
        Query *q = db->sql("select getdate() datetime_type, current_timestamp right_now, 1 integer_type, 0.1 float_type");
        
        try {
            /* Execute SQL query. */
            q->execute();
            /* Print table headers, ie column names. */
            q->rows->printheader();
            cout << "Results:" << endl;
            while (!q->eof()) {
                cout << "| ";
                for (int i=0; i < q->fieldcount; i++) {
                    auto fd = q->fields(i);
                    auto v = fd->to_int();
                    auto int_t = typeid(long).hash_code();
                    const auto& ti = typeid(v);
                    string v_t = ti.name();
                    auto v_h = ti.hash_code();
                    auto v1 = fd->to_double();
                    auto v1_t = typeid(v1).hash_code();
                    auto v2 = fd->to_str();
                    auto v2_t = typeid(v2).hash_code();
                    cout << v2 << " | ";
                }
                cout << endl;
                q->next();
            }
        }
        catch(TDSPP::Exception &e) {
            cerr << e.message << endl;
        }
        delete q;
    }
    catch(TDSPP::Exception &e) {
        cerr << e.message << endl;
    }
    delete db;
    return 0;    
}
