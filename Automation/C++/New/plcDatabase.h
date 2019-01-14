
#include "plcBase.h"

class plcDatabase : public plcBase {
    private:
        sqlite3 *db; 
        string dbName=":memory:";
        int con; // sqlite3 db connection
        
        bool sqlError(int rc, char *err_msg ) ;
        bool dbSetup() ;            // Use default database name.
        
        virtual string getValue(string);
        virtual bool setValue(string, string );         // Short name, string value
    
        virtual string getOldValue(string );               // Short name
        virtual bool setOldValue(string, string );         // Short name, string value
        

    public:
        plcDatabase();
        plcDatabase( string name);
        
        bool initPlc(string ) ;
        void setDBname( string );
        string getDBname( );
        
        bool addIOPoint(string );               // Short name
        bool addIOPoint(string,string );        // Short name, topic
        bool addIOPoint(string,string, string );// Short name, topic, direction
        
        void Out(string);
        virtual bool plcEnd(int);  // Delay in ms
};
