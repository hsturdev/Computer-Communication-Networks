# Tiny TCP
* TCP is the a reliable transport layer protocol that transfer data over the Internet, most of the files transfers on the Internet using TCP, another most common transport layer protocol is UDP, but it considers as unreliable and it mostly used in live streaming.
* This project implement a **reliable transport layer protocol(tinytcp)**.
* tinytcp deliver a stream of bytes reliably and in-order to applications over the network
* Unlike complete version of TCP, tinytcp does not have flow control and congestion control
* The testing occurs on the **virtual envirnment on local host**, but after applying some setings **it can also be used in the real world envirment**, so people can use it to transfer files from one machine to another remotely.
* This is one of the my course project, please **read only, do not share, copy and distribute it**, especially if you are student who is taking the same/similar course

# Demo
https://user-images.githubusercontent.com/70169080/187096013-400c447e-f25c-47d3-9f95-772eeab3d263.mp4

* Two terminal represents different machines, where server machine stores and send files, client machine request and receive files
* **sendfiles** folder represents the folder that stores all files that server machine want to be sent
* **recvfiles** folder represents the folder that client machine want to store the files from the server machine
* left terminal starts the server by typing```./bin/tinytcp server``` + ```packet drop probability```, ```packet drop probability``` means the probability that packet from the server get droped, because we know packet can get drop/lost when tranfering data over the Internet, in the demo I use ```10``` as server packet drop probability
* right terminal is the client machine, it request files from server by typing ```./bin/tinytcp client``` + ```packet drop probability``` + ```filenames```, ```packet drop probability``` means the probability that packet get droped from the client side, I use ```10``` in the demo, ```filenames``` is all the files that client want to request from the server, I use 3 different files on the demo
* After requesting, server starts send files to the client, and this is what transport layer protocol does, transport data.
* Finally after all the files were transfered, I use ```diff``` to verify the received files are identical to the files on the server side, this is what a reliable transport layer protocol should be, reliable transport layer protocol need to make sure data actually reach the destination in correct order, but unreliable transport layer protocol like UDP does not care if the data are received by the client. 
