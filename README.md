Edongle
=======

UMJI VE373 project in 2014 summer semester
#Edongle
-------------------------------------------------
Dongle which enables user to do their own configurations.

#Function
---------------------------------------------------
Function:
*1. A movable pocket device which can store the secret, classified or confidential, different kind of level info (base on s-AES encryption)
*2. Provide interface and create secure bridge between user software 
*3. Provide encryption and decryption service (parallel )

# How to run midware
-------------------------------------------------
make sure you run connect the PIC32
the type in terminal `sudo midware`  (need root permission)


# How to use client communication
-------------------------------------------------
We provide test client in /client
run the client `client`


# How to use encryption service
-------------------------------------------------
If you want encrypt a file
you should type `sudo midware cleartext.txt Encrypted.txt 1`

If you want to decrypte a file 
you should type `sudo midware Encrypted.txt outClear.txt 2'


# How to use browser extension module
-------------------------------------------------
to load the Chrome extension, open the Chrome
<pre>
option->Tool->extension
Load the developing tools
</pre>

to configure or store the web
<pre>
right click the icon on tool bar
input the info, click save, wait it stores info in PIC32 , then close 
</pre>


#Support 
-------------------------------------------
* Ubuntu 12.04

#Development tool 
-------------------------------------
* MPLAB
* QtCreator

#library and used open source project
------------------
*salamanderrex/jtorrent
*openssl/openssl
*mortzdk/Websocket
*jiaxiluo/sakai_plus
*open-source-parsers/jsoncpp
*SHA-1


