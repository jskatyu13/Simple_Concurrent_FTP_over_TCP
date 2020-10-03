# Simple_Concurrent_FTP_over_TCP

This FTP program contains a FTP server and FTP clients which supports at least 4 connections over TCP to the server. 

The client supports the following commands: 'help', 'lls', 'lpwd','lcd','get','bye' and 'exit', to perform various operations on the client side for listing commands, listing files, showing current directory, changing directory, downloading files, and  exiting the client process.  

The server supports to accept various requests from multiple clients at the same time by creating multithreading in the server side.  

The error handling is implemented at various stages at both server and client sides.  
