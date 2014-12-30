#pragma once
// common header for custom pages

enum PAGE { LOGINPAGE = 0, SIGNUPPAGE, ABOUTPAGE, SVCONNECTPAGE, HOMEPAGE };

enum PAGECONTROL{
    LP_LOGINBUTTON = 0, LP_USERNAMETEXT, LP_PASSWORDTEXT,   // Login page controls
    SP_SIGNUPBUTTON, SP_USERNAMETEXT, SP_PASSWORDTEXT, SP_CONFIRMPASSWORDTEXT, // Signup page controls
    SCP_CONNECTBUTTON, SCP_NAMETEXT, SCP_IPTEXT, SCP_GROUPIDTEXT, // Server connect Page controls
    HP_SENDBUTTON, HP_MSGTEXT, HP_MSGHISTORYLABEL  // Home page controls
};