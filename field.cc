/*
 * $Id: field.cc,v 2.7 2005/10/20 11:24:12 martin Exp $
 * Copyright (c) 2004, 2005, Voidsoft AB
 */

#include <iostream>
#include "field.hh"
#include "tdspp.hh" // for TDSPP::Exception

/** Class Field */
/** Constructor */
Field::Field()
    : data(NULL) {
}

Field::Field(string name, int size, int datatype_) {
    colname = name;
    data = new char[size];
    data[0] = 0;
    datatype = datatype_;
}

/** Destructor */
Field::~Field() {
    delete [] data; 
}

string Field::tostr() {
    if (!data) throw TDSPP::Exception("Field::tostr: Data not initialized");
    return data;
}

/* Not implemented */
long Field::toint() {
    if (!data) throw TDSPP::Exception("Field::toint: Data not initialized");
    return 0;
}

/** Class Rows */
/** Constructor */
Rows::Rows()
    : currentrow(0) {
}

/** Destructor */
Rows::~Rows() {
    clear();
}

void Rows::clear(void) {
    while (rows.size()) {
        while (rows[rows.size()-1].size()) {
            delete rows[rows.size()-1].back();
            rows[rows.size()-1].pop_back();
        }
        rows.pop_back();
    }
}

void Rows::printheader(void) {
    cout << "| ";
    for (unsigned int i=0; i < rows[currentrow].size(); i++) {
        cout << rows[currentrow][i]->colname << " | ";
    }
    cout << endl;
}

void Rows::print(void) {
    cout << "| ";
    for (unsigned int i=0; i < rows[currentrow].size(); i++) {
         cout << rows[currentrow][i]->tostr() << " | ";
    }
    cout << endl;
}
