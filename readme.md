There are many shopify scrapers out there but I doubt any of them are using multithreading. With this in mind, I thought I could use what I learned in ECE 252 at the University of Waterloo to build a shopify scraper that outperforms existing ones.

I have modelled this program as a modified producer-consumer problem. I say modified because the shared buffer does not have an upper bound on the number of spaces available. Instead, only one general semaphore, `items`, is required, which keeps track of the number of full spaces in the buffer. When a producer thread has done it's job and pushed the result to the buffer, it will post on `items`. At the same time, consumer threads will be waiting on `items` so that they can consume what was produced.

Each producer thread handles a different page of the `/products.json?page=X` endpoint.


### Installation
Run `make` to create the executable `main` file.

### Usage
Run `./main <BASE_URL>` where `<BASE_URL>` should be replaced with a shopify website base url. For example,
`./main http://example.com`.
