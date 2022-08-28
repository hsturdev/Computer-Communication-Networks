# C++ Based Server
This project was inspired by one of my college course and [@qinguoyi](https://github.com/qinguoyi/TinyWebServer). On my college course, I implemented a [HTTP web server](https://github.com/tlistudents/Computer-Communication-Networks) and study the computer network, this project helps me get a better understanding of computer communication and socket programming.
You can find my other Computer Communication Network related projects [here](https://github.com/tlistudents/Computer-Communication-Networks).

* Function Intro 
	* A server based on C++
	* Support user registration, it links to MySQL to store user's informaction
	* This server can serve local contents(images, videos, etc.) to the registrated users
	* This server can serve multiple clients at the same time
* Tech used
	* Concurrency model: thread pool + non-blocking socket + epoll (edge-triggered mode and level triggered mode) + event handling
	* Use state machine to handle HTTP request, support POST/GET request
	* Synchronous/asynchronous log

# Demo
https://user-images.githubusercontent.com/70169080/187099051-63d169fe-72a2-4575-b428-7fb7b4201eb4.mp4

