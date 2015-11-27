/*
 * $Id: tdspp.hh,v 2.8 2005/10/20 11:24:12 martin Exp $
 * Copyright (c) 2004, 2005, Voidsoft AB
 */

#ifndef VOIDSOFT_TDSPP_TDSPP_H
#define VOIDSOFT_TDSPP_TDSPP_H

#include <ctpublic.h>
#include <string>
#include <exception>
#include "query.hh"

using namespace std;

class Query;



/** TDS++ class */
class TDSPP {
    friend class Query;
public:
    /** Constructor */
    TDSPP();
    /** Destructor */
    ~TDSPP();
    /** Connect to database. */
    void connect(const string& server, 
                 const string& username, 
                 const string& password);
    /** Disconnect from database. */
    void disconnect();
    /** Execute a command */
    void execute(const string& sql);
    /** Query */
    Query *sql(const string& sql);

    /** Exception class for tds++ */
    class Exception : public exception {
    public :
        /** Error message */
        string message;
        
        /** Constructor */
        explicit Exception(const string& msg="") {message = msg;}
        /** Destructor */
        virtual ~Exception() throw() { }
    };    
private:
    void login();
    string server, username, password;
    int verbose;
    CS_CONTEXT *ctx;
    CS_CONNECTION *conn;
    CS_COMMAND *cmd;
    CS_RETCODE init();
};

#endif /* VOIDSOFT_TDSPP_TDSPP_H */
