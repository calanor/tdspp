/*
 * $Id: field.hh,v 2.7 2005/10/20 11:24:12 martin Exp $
 * Copyright (c) 2004, 2005, Voidsoft AB
 */

#ifndef VOIDSOFT_TDSPP_FIELD_HH
#define VOIDSOFT_TDSPP_FIELD_HH

#include <string>
#include <vector>
#include <memory>

using namespace std;

/** Field class. AKA column. */
class Field {
    friend class Query;
public:
    /** Name of the column */
    string colname;

    /** Constructor */
    Field();
    /** Constructor */
    Field(string name, int size, int datatype_);
    /** Destructor */
    ~Field();

    // CS type reference: https://github.com/FreeTDS/freetds/blob/master/include/cspublic.h
    /** Return string value of a field. */
    string   to_str(void);
    /** Return integer value of a field. */
    long     to_int(void);

    double to_double();
    /** Return field returned length */
    int      getDataLength() { return datalength; }

    int      getDataType() { return datatype; }
    
private:
    char* data;
    int   datalength;
    int   datatype;
};

/** Row class */
class Rows {
    friend class Query;
public:
    /** List of fields type */
    typedef vector<Field*> FieldList;
    /** List of rows type */
    typedef vector<FieldList> RowList;
    /** Iterator for list of fields type */
    typedef FieldList::iterator FIte;
    /** List of rows */
    RowList rows;
    
    /** Constructor */
    Rows();
    /** Destructor */
    ~Rows();
    /** Prints a header for a table to stdout. Prints all column names. */
    void printheader();
    /** Prints a row to stdout. */
    void print();
    
private:
    void clear(void);
    int currentrow;  
};

#endif /* VOIDSOFT_TDSPP_FIELD_HH */
