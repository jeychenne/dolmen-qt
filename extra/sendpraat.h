#ifndef SENDPRAAT_H
#define SENDPRAAT_H



#if defined(__cplusplus)
extern "C" {
#endif


/*
 * The way to call the sendpraat subroutine from another program.
 */
char *sendpraat (void *display, const char *programName, long timeOut, const char *text);
wchar_t *sendpraatW (void *display, const wchar_t *programName, long timeOut, const wchar_t *text);
/*
 * Parameters:
 * 'display' is the Display or GdKDisplay pointer, which will be available if you call sendpraat from an X11 or GTK program.
 *    If 'display' is NULL, sendpraat will open the display by itself, and close it after use.
 *    On Windows and Macintosh, sendpraat ignores the 'display' parameter.
 * 'programName' is the name of the program that receives the message.
 *    This program must have been built with the Praat shell (the most common such programs are Praat and ALS).
 *    On Unix, the program name is usually all lower case, e.g. "praat" or "als", or the name of any other program.
 *    On Windows, you can use either "Praat", "praat", or the name of any other program.
 *    On Macintosh, 'programName' must be "Praat", "praat", "ALS", or the Macintosh signature of any other program.
 * 'timeOut' is the time (in seconds) after which sendpraat will return with a time-out error message
 *    if the receiving program sends no notification of completion.
 *    On Unix and Macintosh, the message is sent asynchronously if 'timeOut' is 0;
 *    this means that sendpraat will return OK (NULL) without waiting for the receiving program
 *    to handle the message.
 *    On Windows, the time out is ignored.
 * 'text' contains the contents of the Praat script to be sent to the receiving program.
 */

#if defined(__cplusplus)
}
#endif




#endif /* End of file sendpraat.h */
