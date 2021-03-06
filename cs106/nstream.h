/*
 * File: nstream.h
 * Last modified on Sat Jan 31 00:00:00 2009 by htiek
 * --------------------------------------------------
 * This file is the interface for a stream object that lets you
 * send and receive data across a network connection.  The class,
 * nstream, is similar to the ifstream and ofstream classes,
 * except that it allows for both reading and writing.
 *
 * To use this class, you must first create an instance of an
 * nstream object by declaring
 *
 *      nstream myStream;
 *
 * At this point, you need to initialize the network connection.
 * To do this, you will either need to tell the stream to wait for
 * an incoming connection from a remote computer, or will need to
 * open a connection to a remote computer.
 *
 * To wait for an incoming connection, you use the "open" function,
 * which accepts as input a "port number."  This number is an
 * application-specific value that lets your program filter out
 * other incoming requests for other programs.  For example, if you
 * listen for incoming connections on port 1337, then other
 * applications will have to make a connection on port 1337 in order
 * for the nstream to receive the transmission.  Port numbers are
 * stored as shorts, so their values range between 0 and 32767.
 *
 * For example, to have the stream listen on port 1337 for an
 * incoming connection, you would write
 *
 *      myStream.open(1337);
 *
 * The call to open will wait indefinitely for an incoming
 * connection and will return only when a connection has been
 * received or if a network error has occurred.  You can use
 * the is_open() or fail() member functions to check if the
 * operation succeeded.  For example:
 *
 *		nstream myStream;
 *		myStream.open(1337);
 *		if(!myStream.is_open()) { .. handle error .. }
 *
 * If you'd like to use the nstream to connect to a remote
 * computer, you use a different version of the open function
 * which accepts as input two parameters, a host name and a
 * port number.  The port number is used as described above
 * to make sure that the right application on the receiving
 * machine accepts the connection.  The host name can either
 * be an IP address (e.g. "137.42.31.27") or the name of a
 * computer on the Internet (e.g. "www.google.com").
 *
 * Like the other version of "open," the two-argument version
 * of "open" will return only after a connection has been
 * established or the connection failed.  Use is_open() and
 * fail() to check for errors.
 *
 * Here's some sample code to illustrate how to make a
 * connection:
 *
 *		nstream myStream;
 *		myStream.open("137.42.31.27", 1337);
 *		if(!myStream.is_open()) { .. error .. }
 *
 * Once you have established the connection, you can use the
 * nstream as you would any other stream class (e.g. ifstream,
 * cout, ofstream).  Any data you transmit will be sent across
 * the connection.  When reading data, the stream will wait for
 * the remote machine to send data before returning.  This means
 * that you should be careful not to read any data unless you
 * are sure that the remote machine has sent something.
 *
 * A minor point to be aware of is that the nstream class is
 * buffered and thus may not immediately send the data that is
 * passed into it.  To ensure that the data you write is sent,
 * make sure to flush the stream using the "endl" manipulator
 * or the flush() member function, as shown here:
 *
 * myStream << "This is some text." << endl; // Flushed by endl
 * myStream << "This is some text!";
 * myStream.flush();
 *
 * When using the nstream object, be sure to periodically check
 * that the stream is not in a fail state by using the .fail()
 * member function.  Because the network connection might fail
 * at any point, you should never assume that any read or write
 * has completed successfully and should be prepared to handle
 * errors gracefully.
 */

#ifndef NStream_Included
#define NStream_Included

#include "genlib.h"
#include <istream> // For iostream
#include <memory>  // For auto_ptr

class SocketStreambuf; // Implementation-specific.
class nstream: public iostream
{
public:
	/* Constructor: nstream()
	 * Usage: nstream myStream;
	 * --------------------------------
	 * Creates a new nstream object that is not connected to any
	 * computer.  Use one of the open() functions to connect to
	 * another computer.
	 */
	nstream();

	/* Constructor: nstream(short portNum)
	 * Usage: nstream myStream(137);
	 * -----------------------------------------
	 * Creates a new nstream object that waits on the specified
	 * port for an incoming connection.  This is equivalent to
	 * creating an empty nstream and then calling .open() and is
	 * provided as a convenience.
	 */
	explicit nstream(short portNum);

	/* Constructor: nstream(string hostName, short portNum)
	 * Usage: nstream myStream("127.0.0.1", 137);
	 * -----------------------------------------
	 * Creates a new nstream object that connects to the specified
	 * computer on the specified port.  This is equivalent to
	 * creating an empty nstream and then calling .open() and is
	 * provided as a convenience.
	 */
	nstream(string hostName, short portNum);

	/* Destructor: ~nstream()
	 * ------------------------------------------
	 * Cleans up the nstream object and closes the connection.
	 * Any unsent data will be sent.
	 */
	~nstream();

	/* Function: open(string hostName, short portNum)
	 * Usage: myStream.open("127.0.0.1", 137);
	 * ------------------------------------------
	 * Opens a connection to the specified computer on the given port.
	 * If a connection already exists or the connection fails, puts the
	 * stream into a fail state.
	 */
	void open(string hostName, short portNum);

	/* Function: open(short portNum)
	 * Usage: myStream.open(137);
	 * ------------------------------------------
	 * Waits for a computer to connect to the specified port.
	 * If a connection already exists or the connection fails, puts the
	 * stream into a fail state.
	 */
	void open(short portNum);

	/* Function: is_open()
	 * Usage: if(!myStream.is_open()) { .. error .. }
	 * ----------------------------------------------
	 * Returns whether a valid connection exists to the
	 * remote machine.
	 */
	bool is_open() const;

	/* Function: close()
	 * Usage: myStream.close();
	 * -----------------------------------------------
	 * Closes any open connections and flushes unsent
	 * data.  If the stream is not open, puts the stream
	 * into a fail state.
	 */
	void close();

private:
	/* This stream buffer is used for the actual connection. */
	const auto_ptr<SocketStreambuf> connection;
};

#endif
