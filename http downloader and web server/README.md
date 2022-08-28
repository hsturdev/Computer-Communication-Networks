# HTTP Downloader
* Implement a HTTP Web Page Downloader(which is similar to "wget" but with limited function)
* This is one of the my course project, please **read only, do not share, copy and distribute it**, especially if you are student who is taking the same/similar course 
# Demo
https://user-images.githubusercontent.com/70169080/187097413-43249f4d-027c-45f1-98c7-564567fd1eef.mp4
* As you can see the downloader download different types of files(.pdf, .txt, .tar, etc.) on gun.org

---

# HTTP Web Server
* This server can serve static contents. like html, css, js, etc.
* It can also serve the dynamic content, which in the demo is the images on the database, when client search the images on the search bar, the server link to the databse and find the request images, and return the images to client if found, otherwise display 404 page
* This is one of the my course project, please **read only, do not share, copy and distribute it**, especially if you are student who is taking the same/similar course 
# Demo
https://user-images.githubusercontent.com/70169080/187097494-79609c70-570c-4326-a5f9-fe627a36daa4.mp4
* The demo first opens the database server on one terminal
* It then opens the web server on the other terminal
* The web server will start serve the website, the testing environment is localhost, but if I got a public IP(eg, rent a VPS from AWS or somewhere else), this web server can also serve the content to everyone in the world
* The webserver is linking to the database server, the databse server stores some cat images
* When client search those image on the search bar, the server then find the images on the databse, and retunr it to the client
