/*
 * $Id: query.hh,v 2.9 2005/10/20 11:24:12 martin Exp $
 * Copyright (c) 2004, 2005, Voidsoft AB
 */

#ifndef VOIDSOFT_TDSPP_QUERY_HH
#define VOIDSOFT_TDSPP_QUERY_HH

#include <string>
#include "tdspp.hh"
#include "field.hh"

using namespace std;

class TDSPP;

/** Query class */
class Query {
public: 
    /** SQL command */
    string command;
    /** Rows */
    Rows *rows;
    /** Number of fields per row. */
    CS_INT fieldcount;
    /** Number of rows in a result set. */
    CS_INT rowcount;
    
    /** Constructor */
    Query(TDSPP* tds);
    /** Destructor */
    ~Query();
    /** Execute an SQL query where a result is expected. */
    void execute();
    /** Iterate to next row in a result set. */
    void next();
    /** Go to the first row in a result set. */
    void first();
    /** Return the columnvalue i for the current row. */
    Field *fields(int i); 
    /** Return the columnvalue s for the current row. */
    Field *fields(string s);
    /** Check if we've reached the end of the result set. */
    bool eof();

    /** Overloaded [] operator. Used to specify index in a row. */
    string operator[](const char *);
    /** Overloaded [] operator. Used to specify index in a row. */
    string operator[](string);

private:
    TDSPP* TDS;   
    bool endoffile;
    void init();
    void addrow();
    bool addfield();
    void getrc();
    void getresults();  
};

#endif /* VOIDSOFT_TDSPP_QUERY_HH */
