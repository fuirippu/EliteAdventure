#ifndef OBCOMP_H
#define OBCOMP_H

/////////////////////////////////////////////////////////////////////////////
/// Interface for on-board computer messages
///
/// Scrolling display for 15 most recent messages
/////////////////////////////////////////////////////////////////////////////


extern const char *strNewShipMsg;


void obc_message(const char *msg, int col);

void obc_display();
void obc_refresh();

const char *obc_ship_name(int type);


#endif		// #ifndef OBCOMP_H
