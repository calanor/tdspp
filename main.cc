/*
 * $Id: main.cc,v 1.6 2005/10/20 11:27:42 martin Exp $
 * Copyright (c) 2004, 2005, Voidsoft AB
 */

#include <stdio.h>
#include <string.h>
#include <ctpublic.h>
#include <iostream>
#include "tdspp.hh"

int main(int argc, char **argv) {
    
    TDSPP *db = new TDSPP();
    try {
        /* Connect to database. */
        db->connect("localhost:1433", "admin", "12345678");
        /* Execute command. */
        db->execute("use master");
        /* Create query. */
        Query *q = db->sql("select current_timestamp right_now, 1 integer_type, 0.1 float_type");
        
        try {
            /* Execute SQL query. */
            q->execute();
            /* Print table headers, ie column names. */
            q->rows->printheader();
            while (!q->eof()) {
                cout << "| ";
                for (int i=0; i < q->fieldcount; i++) {
                    auto fd = q->fields(i); 
                    cout << q->fields(i)->tostr() << " | ";
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
