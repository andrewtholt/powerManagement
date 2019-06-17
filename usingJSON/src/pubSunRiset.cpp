
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <mosquitto.h>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>

#define SUNRISE "/test/sunrise"
#define SUNSET  "/test/sunset"

using namespace std;
using json = nlohmann::json;

/* A macro to compute the number of days elapsed since 2000 Jan 0.0 */
/* (which is equal to 1999 Dec 31, 0h UT)                           */

#define days_since_2000_Jan_0(y,m,d) \
    (367L*(y)-((7*((y)+(((m)+9)/12)))/4)+((275*(m))/9)+(d)-730530L)

/* Some conversion factors between radians and degrees */

#ifndef PI
#define PI        3.1415926535897932384
#endif

#define RADEG     ( 180.0 / PI )
#define DEGRAD    ( PI / 180.0 )

/* The trigonometric functions in degrees */

#define sind(x)  sin((x)*DEGRAD)
#define cosd(x)  cos((x)*DEGRAD)
#define tand(x)  tan((x)*DEGRAD)

#define atand(x)    (RADEG*atan(x))
#define asind(x)    (RADEG*asin(x))
#define acosd(x)    (RADEG*acos(x))
#define atan2d(y,x) (RADEG*atan2(y,x))


/* Following are some macros around the "workhorse" function __daylen__ */
/* They mainly fill in the desired values for the reference altitude    */
/* below the horizon, and also selects whether this altitude should     */
/* refer to the Sun's center or its upper limb.                         */


/* This macro computes the length of the day, from sunrise to sunset. */
/* Sunrise/set is considered to occur when the Sun's upper limb is    */
/* 35 arc minutes below the horizon (this accounts for the refraction */
/* of the Earth's atmosphere).                                        */
#define day_length(year,month,day,lon,lat)  \
    __daylen__( year, month, day, lon, lat, -35.0/60.0, 1 )

/* This macro computes the length of the day, including civil twilight. */
/* Civil twilight starts/ends when the Sun's center is 6 degrees below  */
/* the horizon.                                                         */
#define day_civil_twilight_length(year,month,day,lon,lat)  \
    __daylen__( year, month, day, lon, lat, -6.0, 0 )

/* This macro computes the length of the day, incl. nautical twilight.  */
/* Nautical twilight starts/ends when the Sun's center is 12 degrees    */
/* below the horizon.                                                   */
#define day_nautical_twilight_length(year,month,day,lon,lat)  \
    __daylen__( year, month, day, lon, lat, -12.0, 0 )

/* This macro computes the length of the day, incl. astronomical twilight. */
/* Astronomical twilight starts/ends when the Sun's center is 18 degrees   */
/* below the horizon.                                                      */
#define day_astronomical_twilight_length(year,month,day,lon,lat)  \
    __daylen__( year, month, day, lon, lat, -18.0, 0 )


/* This macro computes times for sunrise/sunset.                      */
/* Sunrise/set is considered to occur when the Sun's upper limb is    */
/* 35 arc minutes below the horizon (this accounts for the refraction */
/* of the Earth's atmosphere).                                        */
#define sun_rise_set(year,month,day,lon,lat,rise,set)  \
    __sunriset__( year, month, day, lon, lat, -35.0/60.0, 1, rise, set )

/* This macro computes the start and end times of civil twilight.       */
/* Civil twilight starts/ends when the Sun's center is 6 degrees below  */
/* the horizon.                                                         */
#define civil_twilight(year,month,day,lon,lat,start,end)  \
    __sunriset__( year, month, day, lon, lat, -6.0, 0, start, end )

/* This macro computes the start and end times of nautical twilight.    */
/* Nautical twilight starts/ends when the Sun's center is 12 degrees    */
/* below the horizon.                                                   */
#define nautical_twilight(year,month,day,lon,lat,start,end)  \
    __sunriset__( year, month, day, lon, lat, -12.0, 0, start, end )

/* This macro computes the start and end times of astronomical twilight.   */
/* Astronomical twilight starts/ends when the Sun's center is 18 degrees   */
/* below the horizon.                                                      */
#define astronomical_twilight(year,month,day,lon,lat,start,end)  \
    __sunriset__( year, month, day, lon, lat, -18.0, 0, start, end )


/* Function prototypes */

double __daylen__( int year, int month, int day, double lon, double lat,
        double altit, int upper_limb );

int __sunriset__( int year, int month, int day, double lon, double lat,
        double altit, int upper_limb, double *rise, double *set );

void sunpos( double d, double *lon, double *r );

void sun_RA_dec( double d, double *RA, double *dec, double *r );

double revolution( double x );

double rev180( double x );

double GMST0( double d );


void usage() {
    printf("pubSunRiset -h|-v -o <offset> -c <cfgFile> -r <sunrise topic> -s <sunset topic>\n\n");
    printf("\t-h\t\tHelp.\n");
    printf("\t-v\t\tVerbose.\n");
    printf("\t-c <cfgFile>\tUse config file.\n");
    printf("\t-o <offset>\tAdd offset (minutes) to sun rise time, and subtract from sunrise time.\n");
    printf("\t-r <topic>\tOverride config file sunrise topic.\n");
    printf("\t-s <topic>\tOverride config file sunset topic.\n");

}

string getFromJson(json j, vector<string> p, string defaultValue) {

    string v;
    string t;
    int len = p.size();

//    string dump = j.dump(4);
//    cout << dump << endl;

    try {
        switch(len) {
            case 0:
                break;
            case 1:
                v=j[ p[0]  ];
                break;
            case 2:

                {
                    string a = p[0];
                    string b = p[1];

                    v=j[a][b];
                }

                break;
            case 3:
                v=j[p[0]] [p[1]] [p[2]];
                break;
            default:
                cerr << "getFromJson:FATAL Error, too many parameters, Max is 3" << endl;
                exit(4);
                break;
        }
    } catch (json::type_error &e) {
        v = defaultValue;
    }

    return v;
}

string addOffset(int hh, int mm, int off) {

    int ahh, amm;

    int minutes = (hh * 60) + mm;

    minutes += off;

    ahh = minutes / 60;
    amm = minutes % 60;

    string res = to_string(ahh) + ":" + to_string(amm);

    return res;
}

int main(int argc, char *argv[]) {
    int opt;
    int year,month,day, dst;
    double lon, lat;
    double daylen, civlen, nautlen, astrlen;
    double rise, set, civ_start, civ_end, naut_start, naut_end;
    double astr_start, astr_end;
    int    rs, civ, naut, astr;
    char buf[80];

    int riseHH, riseMM;
    int setHH, setMM;

    bool verbose=false;

    string cfgFile = "/etc/mqtt/bridge.json";
    json config ;

    string sunriseTopic;
    string sunsetTopic;

    bool useGMT = true;
    int offset=0;

    while((opt = getopt(argc, argv, "c:hvr:s:o:")) != -1) {
        switch(opt) {
            case 'c':
                cfgFile = optarg;
                break;
            case 'h':
                usage();
                exit(1);
                break;
            case 'v':
                verbose=true;
                break;
            case 'r':   // Sunrise
                sunriseTopic=optarg;
                break;
            case 's':   // Sunset
                sunsetTopic=optarg;
                break;
            case 'o':
                offset = atoi(optarg);
                break;
        }
    }

    if(access(cfgFile.c_str(), R_OK) < 0) {
        cerr << "FATAL: Cannot access config file " + cfgFile << endl;
        exit(2);
    }

    ifstream cfgStream( cfgFile );
    config = json::parse(cfgStream);

    string mosquittoHost  = getFromJson(config, {"local","name"}, "127.0.0.1");
    string mqttPortString = getFromJson(config, {"local","port"}, "1883");
    int mosquittoPort = stoi( mqttPortString );

    vector<string> jsonParams;

    if ( sunriseTopic == "" ) {
        sunriseTopic=getFromJson(config, { "topics","sunrise" }, "/test/SUNRISE") ;
    }


    if ( sunsetTopic == "" ) {
        sunsetTopic =getFromJson(config, { "topics","sunset" }, "/test/SUNSET") ;
    }

    string longString = getFromJson(config, { "location","long" }, "0.48650" ) ;
    string latString  = getFromJson(config, { "location","lat"  }, "0.163598" ) ;

    lon = stod( longString );
    lat = stod( latString );

    if( verbose ) {
        cout << "Config File : " << cfgFile << endl;
        cout << "MQTT Broker : " << mosquittoHost << endl;
        cout << "MQTT Port   : " << mosquittoPort << endl;
        cout << "Longtitude  : " << longString << endl;
        cout << "Latitude    : " << latString << endl;
    }

    int rc=-1;
    int keepalive=0;

    struct mosquitto *mosq = mosquitto_new("dispatch", true, NULL);

    if( mosq) {
        rc=mosquitto_connect(mosq, mosquittoHost.c_str(), mosquittoPort, keepalive);
    } else {
        cerr << "FATAL: Failed to connect to MQTT Broker" << endl;
        exit(3);
    }

    time_t now = time(NULL);
    struct tm *tm = localtime( &now );

    year = tm->tm_year + 1900 ;
    month = tm->tm_mon+1;
    day = tm->tm_mday;
    dst = tm->tm_isdst;

    /*

    time_t rawtime;
    struct tm timeinfo;  // get date and time info
    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);
    int isdaylighttime = timeinfo.tm_isdst;
    */


    daylen  = day_length(year,month,day,lon,lat);
    civlen  = day_civil_twilight_length(year,month,day,lon,lat);
    nautlen = day_nautical_twilight_length(year,month,day,lon,lat);
    astrlen = day_astronomical_twilight_length(year,month,day, lon,lat);


    rs   = sun_rise_set( year, month, day, lon, lat, &rise, &set );

    riseHH=(int)rise ;
    riseMM=(int)((rise - riseHH) * 60);

    setHH=(int)set ;
    setMM=(int)((set - setHH) * 60);


    string sunRise;
    string sunSet;

    if( dst == 1 ) {
        riseHH++;
        setHH++;
    }

    if(offset > 0) {
        sunRise = addOffset(riseHH, riseMM, offset);
        sunSet = addOffset(setHH, setMM, -offset);
    } else {
        sunRise = to_string(riseHH) + ":" + to_string(riseMM) ;
        sunSet  = to_string(setHH) + ":" + to_string(setMM) ;
    }

    if(verbose) {
        printf("====================\n");
        printf("Day     %02d\n", day);
        printf("Month   %02d\n", month);
        printf("Year    %4d\n\n", year);

        printf("Offset           : %02d\n", offset);
        cout <<"Sunrise :" << sunRise << endl; 
        cout <<"Sunset  :" << sunSet  << endl; 

                printf("Sunrise %02d:%02d\n", riseHH, riseMM);
                printf("Sunset  %02d:%02d\n", setHH, setMM);
        printf("====================\n");
    }

    mosquitto_publish(mosq, NULL, (char *)sunriseTopic.c_str(), sunRise.length(),
            (char *)sunRise.c_str(), 1,true) ;

    mosquitto_publish(mosq, NULL, (char *)sunsetTopic.c_str(), sunSet.length(),
            (char *)sunSet.c_str(), 1,true) ;

    rc=mosquitto_loop(mosq,-1,1);

    return rc;
}


/* The "workhorse" function for sun rise/set times */

int __sunriset__( int year, int month, int day, double lon, double lat,
        double altit, int upper_limb, double *trise, double *tset )
    /***************************************************************************/
    /* Note: year,month,date = calendar date, 1801-2099 only.             */
    /*       Eastern longitude positive, Western longitude negative       */
    /*       Northern latitude positive, Southern latitude negative       */
    /*       The longitude value IS critical in this function!            */
    /*       altit = the altitude which the Sun should cross              */
    /*               Set to -35/60 degrees for rise/set, -6 degrees       */
    /*               for civil, -12 degrees for nautical and -18          */
    /*               degrees for astronomical twilight.                   */
    /*         upper_limb: non-zero -> upper limb, zero -> center         */
    /*               Set to non-zero (e.g. 1) when computing rise/set     */
    /*               times, and to zero when computing start/end of       */
    /*               twilight.                                            */
    /*        *rise = where to store the rise time                        */
    /*        *set  = where to store the set  time                        */
    /*                Both times are relative to the specified altitude,  */
    /*                and thus this function can be used to compute       */
    /*                various twilight times, as well as rise/set times   */
    /* Return value:  0 = sun rises/sets this day, times stored at        */
    /*                    *trise and *tset.                               */
    /*               +1 = sun above the specified "horizon" 24 hours.     */
    /*                    *trise set to time when the sun is at south,    */
    /*                    minus 12 hours while *tset is set to the south  */
    /*                    time plus 12 hours. "Day" length = 24 hours     */
    /*               -1 = sun is below the specified "horizon" 24 hours   */
    /*                    "Day" length = 0 hours, *trise and *tset are    */
    /*                    both set to the time when the sun is at south.  */
    /*                                                                    */
    /**********************************************************************/
{
    double  d,  /* Days since 2000 Jan 0.0 (negative before) */
            sr,         /* Solar distance, astronomical units */
            sRA,        /* Sun's Right Ascension */
            sdec,       /* Sun's declination */
            sradius,    /* Sun's apparent radius */
            t,          /* Diurnal arc */
            tsouth,     /* Time when Sun is at south */
            sidtime;    /* Local sidereal time */

    int rc = 0; /* Return cde from function - usually 0 */

    /* Compute d of 12h local mean solar time */
    d = days_since_2000_Jan_0(year,month,day) + 0.5 - lon/360.0;

    /* Compute the local sidereal time of this moment */
    sidtime = revolution( GMST0(d) + 180.0 + lon );

    /* Compute Sun's RA, Decl and distance at this moment */
    sun_RA_dec( d, &sRA, &sdec, &sr );

    /* Compute time when Sun is at south - in hours UT */
    tsouth = 12.0 - rev180(sidtime - sRA)/15.0;

    /* Compute the Sun's apparent radius in degrees */
    sradius = 0.2666 / sr;

    /* Do correction to upper limb, if necessary */
    if ( upper_limb )
        altit -= sradius;

    /* Compute the diurnal arc that the Sun traverses to reach */
    /* the specified altitude altit: */
    {
        double cost;
        cost = ( sind(altit) - sind(lat) * sind(sdec) ) /
            ( cosd(lat) * cosd(sdec) );
        if ( cost >= 1.0 )
            rc = -1, t = 0.0;       /* Sun always below altit */
        else if ( cost <= -1.0 )
            rc = +1, t = 12.0;      /* Sun always above altit */
        else
            t = acosd(cost)/15.0;   /* The diurnal arc, hours */
    }

    /* Store rise and set times - in hours UT */
    *trise = tsouth - t;
    *tset  = tsouth + t;

    return rc;
}  /* __sunriset__ */



/* The "workhorse" function */


double __daylen__( int year, int month, int day, double lon, double lat,
        double altit, int upper_limb )
    /**********************************************************************/
    /* Note: year,month,date = calendar date, 1801-2099 only.             */
    /*       Eastern longitude positive, Western longitude negative       */
    /*       Northern latitude positive, Southern latitude negative       */
    /*       The longitude value is not critical. Set it to the correct   */
    /*       longitude if you're picky, otherwise set to to, say, 0.0     */
    /*       The latitude however IS critical - be sure to get it correct */
    /*       altit = the altitude which the Sun should cross              */
    /*               Set to -35/60 degrees for rise/set, -6 degrees       */
    /*               for civil, -12 degrees for nautical and -18          */
    /*               degrees for astronomical twilight.                   */
    /*         upper_limb: non-zero -> upper limb, zero -> center         */
    /*               Set to non-zero (e.g. 1) when computing day length   */
    /*               and to zero when computing day+twilight length.      */
    /**********************************************************************/
{
    double  d,  /* Days since 2000 Jan 0.0 (negative before) */
            obl_ecl,    /* Obliquity (inclination) of Earth's axis */
            sr,         /* Solar distance, astronomical units */
            slon,       /* True solar longitude */
            sin_sdecl,  /* Sine of Sun's declination */
            cos_sdecl,  /* Cosine of Sun's declination */
            sradius,    /* Sun's apparent radius */
            t;          /* Diurnal arc */

    /* Compute d of 12h local mean solar time */
    d = days_since_2000_Jan_0(year,month,day) + 0.5 - lon/360.0;

    /* Compute obliquity of ecliptic (inclination of Earth's axis) */
    obl_ecl = 23.4393 - 3.563E-7 * d;

    /* Compute Sun's ecliptic longitude and distance */
    sunpos( d, &slon, &sr );

    /* Compute sine and cosine of Sun's declination */
    sin_sdecl = sind(obl_ecl) * sind(slon);
    cos_sdecl = sqrt( 1.0 - sin_sdecl * sin_sdecl );

    /* Compute the Sun's apparent radius, degrees */
    sradius = 0.2666 / sr;

    /* Do correction to upper limb, if necessary */
    if ( upper_limb )
        altit -= sradius;

    /* Compute the diurnal arc that the Sun traverses to reach */
    /* the specified altitude altit: */
    {
        double cost;
        cost = ( sind(altit) - sind(lat) * sin_sdecl ) /
            ( cosd(lat) * cos_sdecl );
        if ( cost >= 1.0 )
            t = 0.0;                      /* Sun always below altit */
        else if ( cost <= -1.0 )
            t = 24.0;                     /* Sun always above altit */
        else  t = (2.0/15.0) * acosd(cost); /* The diurnal arc, hours */
    }
    return t;
}  /* __daylen__ */


/* This function computes the Sun's position at any instant */

void sunpos( double d, double *lon, double *r )
    /******************************************************/
    /* Computes the Sun's ecliptic longitude and distance */
    /* at an instant given in d, number of days since     */
    /* 2000 Jan 0.0.  The Sun's ecliptic latitude is not  */
    /* computed, since it's always very near 0.           */
    /******************************************************/
{
    double M,         /* Mean anomaly of the Sun */
           w,         /* Mean longitude of perihelion */
           /* Note: Sun's mean longitude = M + w */
           e,         /* Eccentricity of Earth's orbit */
           E,         /* Eccentric anomaly */
           x, y,      /* x, y coordinates in orbit */
           v;         /* True anomaly */

    /* Compute mean elements */
    M = revolution( 356.0470 + 0.9856002585 * d );
    w = 282.9404 + 4.70935E-5 * d;
    e = 0.016709 - 1.151E-9 * d;

    /* Compute true longitude and radius vector */
    E = M + e * RADEG * sind(M) * ( 1.0 + e * cosd(M) );
    x = cosd(E) - e;
    y = sqrt( 1.0 - e*e ) * sind(E);
    *r = sqrt( x*x + y*y );              /* Solar distance */
    v = atan2d( y, x );                  /* True anomaly */
    *lon = v + w;                        /* True solar longitude */
    if ( *lon >= 360.0 )
        *lon -= 360.0;                   /* Make it 0..360 degrees */
}

void sun_RA_dec( double d, double *RA, double *dec, double *r )
    /******************************************************/
    /* Computes the Sun's equatorial coordinates RA, Decl */
    /* and also its distance, at an instant given in d,   */
    /* the number of days since 2000 Jan 0.0.             */
    /******************************************************/
{
    double lon, obl_ecl, x, y, z;

    /* Compute Sun's ecliptical coordinates */
    sunpos( d, &lon, r );

    /* Compute ecliptic rectangular coordinates (z=0) */
    x = *r * cosd(lon);
    y = *r * sind(lon);

    /* Compute obliquity of ecliptic (inclination of Earth's axis) */
    obl_ecl = 23.4393 - 3.563E-7 * d;

    /* Convert to equatorial rectangular coordinates - x is unchanged */
    z = y * sind(obl_ecl);
    y = y * cosd(obl_ecl);

    /* Convert to spherical coordinates */
    *RA = atan2d( y, x );
    *dec = atan2d( z, sqrt(x*x + y*y) );

}  /* sun_RA_dec */


/******************************************************************/
/* This function reduces any angle to within the first revolution */
/* by subtracting or adding even multiples of 360.0 until the     */
/* result is >= 0.0 and < 360.0                                   */
/******************************************************************/

#define INV360    ( 1.0 / 360.0 )

double revolution( double x )
    /*****************************************/
    /* Reduce angle to within 0..360 degrees */
    /*****************************************/
{
    return( x - 360.0 * floor( x * INV360 ) );
}  /* revolution */

double rev180( double x )
    /*********************************************/
    /* Reduce angle to within +180..+180 degrees */
    /*********************************************/
{
    return( x - 360.0 * floor( x * INV360 + 0.5 ) );
}  /* revolution */


/*******************************************************************/
/* This function computes GMST0, the Greenwich Mean Sidereal Time  */
/* at 0h UT (i.e. the sidereal time at the Greenwhich meridian at  */
/* 0h UT).  GMST is then the sidereal time at Greenwich at any     */
/* time of the day.  I've generalized GMST0 as well, and define it */
/* as:  GMST0 = GMST - UT  --  this allows GMST0 to be computed at */
/* other times than 0h UT as well.  While this sounds somewhat     */
/* contradictory, it is very practical:  instead of computing      */
/* GMST like:                                                      */
/*                                                                 */
/*  GMST = (GMST0) + UT * (366.2422/365.2422)                      */
/*                                                                 */
/* where (GMST0) is the GMST last time UT was 0 hours, one simply  */
/* computes:                                                       */
/*                                                                 */
/*  GMST = GMST0 + UT                                              */
/*                                                                 */
/* where GMST0 is the GMST "at 0h UT" but at the current moment!   */
/* Defined in this way, GMST0 will increase with about 4 min a     */
/* day.  It also happens that GMST0 (in degrees, 1 hr = 15 degr)   */
/* is equal to the Sun's mean longitude plus/minus 180 degrees!    */
/* (if we neglect aberration, which amounts to 20 seconds of arc   */
/* or 1.33 seconds of time)                                        */
/*                                                                 */
/*******************************************************************/

double GMST0( double d )
{
    double sidtim0;
    /* Sidtime at 0h UT = L (Sun's mean longitude) + 180.0 degr  */
    /* L = M + w, as defined in sunpos().  Since I'm too lazy to */
    /* add these numbers, I'll let the C compiler do it for me.  */
    /* Any decent C compiler will add the constants at compile   */
    /* time, imposing no runtime or code overhead.               */
    sidtim0 = revolution( ( 180.0 + 356.0470 + 282.9404 ) +
            ( 0.9856002585 + 4.70935E-5 ) * d );
    return sidtim0;
}  /* GMST0 */
