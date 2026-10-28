# WeatherApp: Software Description and Architecture

## 1. Description of the software

WeatherApp is a small web application that shows the current weather for a
city and lets the user save cities as favourites. I built it with C++ backend
services, a simple web frontend, and MongoDB.

The user selects a city from a dropdown list. The application then shows its
temperature, feels-like temperature, description, and wind speed. The weather
information comes from the OpenWeather API. The user can also add a city to
the favourites list, see the saved list, select a favourite, or remove it.

The frontend is made from HTML, CSS, and JavaScript. It is served by Nginx.
Nginx also forwards the requests to the correct backend service, so the
browser only uses one address.

There are two C++ services:

- **WeatherService** listens on port 8080. It receives a city name, calls
  OpenWeather with libcurl, reads the JSON response, and returns the useful
  weather values.
- **FavouritesService** listens on port 8081. It adds, removes, checks, and
  lists favourite cities in MongoDB.

The application can be started with Docker Compose. Only the Nginx container
is exposed to the host on port 3000. The other services use the internal
Docker network. I also created a Kubernetes file so the same application can
run in Minikube or in an Azure Kubernetes Service cluster.

This is a learning project, so it does not have real user accounts. If no
user is supplied, the favourites service uses `user1`. This makes the
application easy to demonstrate, but it would need to be changed for a real
multi-user application.

## 2. Software architecture design

### How the parts work together

The application can be understood as four running parts:

```text
Browser
   |
   v
web-service: Nginx
   |----------------------|
   v                      v
weather-service       favourites-service
   |                      |
   v                      v
OpenWeather API       MongoDB
```

For a weather request, the browser sends a request to Nginx. Nginx forwards it
to WeatherService. WeatherService calls OpenWeather and sends the result back
through Nginx to the browser.

For a favourite request, the browser sends a request to Nginx. Nginx forwards
it to FavouritesService, which reads or changes the data in MongoDB.

In Docker Compose, names such as `weather-service` and `weather-mongo` work as
hostnames inside the Docker network. In Kubernetes, the `Service` objects
provide similar stable names for the Pods. I found this useful because the
browser does not need to know the changing IP address of a container or Pod.

### Components and responsibilities

| Component | Responsibility |
| --- | --- |
| Browser frontend | Shows the page, sends requests, displays weather, and manages the favourites list |
| Nginx (`web-service`) | Serves the HTML, CSS, and JavaScript and forwards API requests |
| WeatherService | Provides `/weather`, calls OpenWeather, and converts the response to the application's JSON format |
| FavouritesService | Provides the favourite endpoints and applies the simple favourite rules |
| FavouritesStore | Keeps the MongoDB-specific code separate from the rest of the favourites service |
| MongoDB (`weather-mongo`) | Stores favourite cities so they survive application container restarts |
| OpenWeather | External service that supplies current weather data |

The frontend files are in `WebService`. The weather HTTP routes and weather
logic are in `WeatherService`. The favourite HTTP routes, service logic, and
MongoDB store are in `FavouritesService`.

### Mapping to microservices

| Application part | Deployed service | Main interface |
| --- | --- | --- |
| Frontend and reverse proxy | `web-service` | HTTP port 80 |
| Weather lookup | `weather-service` | `GET /weather?city=...`, port 8080 |
| Favourite operations | `favourites-service` | `/addFavourite`, `/removeFavourite`, `/isFavourite`, and `/getFavourites`, port 8081 |
| Favourite data | `weather-mongo` | MongoDB port 27017 |

WeatherService and FavouritesService are separate because they do different
jobs. WeatherService does not need the database, and FavouritesService does
not need the weather provider. This makes the code easier for me to follow
and means that one service can be changed without changing the other one.

### Main architecture ideas

I used the following ideas in the design:

- **Separate services:** weather and favourites are separate programs and
  containers.
- **One entry point:** Nginx is the only service exposed to the browser. It
  hides the internal ports and forwards each URL to the right service.
- **Persistent data:** the application containers can be recreated, while
  MongoDB uses a Docker volume or a Kubernetes volume claim for the favourites.
- **Environment configuration:** the OpenWeather API key is passed at runtime
  instead of being copied into the source code or image.
- **Container deployment:** Docker Compose is used for local running, and the
  Kubernetes file describes how to run the same parts in a cluster.

The Kubernetes file uses a `Deployment` for each application service. A
Deployment keeps the requested number of Pods running. It uses a Kubernetes
`Service` to give each group of Pods a stable name and port. MongoDB uses a
`StatefulSet` because it needs stable storage. The web service is exposed
outside the cluster with a `LoadBalancer` service. The weather key is passed
to the weather Pod using a Kubernetes Secret.

## 3. Benefits, challenges, and security

### Benefits

The main benefit is that each part has a small responsibility. The weather
code is not mixed with the MongoDB code, and Nginx handles the connection
between the browser and the internal services. The `FavouritesStore` class
also keeps most MongoDB details in one place.

Docker Compose makes it easier to start the whole application than starting
each program manually. Kubernetes gives me a way to describe the application
in a file and to let Kubernetes recreate a Pod if it stops. The internal
service names also mean that I do not have to hard-code container IP
addresses.

The services could be scaled separately later. For example, more weather
service Pods could be started if weather requests become more common. This
is not needed for the current small project, but the separation makes it
possible.

### Challenges and limitations

This design is more difficult to run than one program. There are several
containers, ports, networks, images, and a database to understand. Building
the favourites image is also slower because it builds the MongoDB C++ driver.

My Kubernetes setup is basic. Each application currently has one replica, so
a service can be unavailable while its only Pod is stopped. MongoDB also
does not have high availability or an automatic backup in this project. The
manifest does not yet include health checks, resource limits, or monitoring.
The use of `mongo:latest` should also be replaced with a fixed version for a
more repeatable deployment.

WeatherService depends on OpenWeather. If the external API is slow, down, or
has reached its rate limit, the user may not receive weather data. The
service currently handles several errors, but it does not yet have request
timeouts, retries, or caching.

There is also no real login system. All requests without a user use `user1`,
so anybody who can access the service can use the same favourites. The add and
remove operations are also currently URL requests rather than carefully
restricted POST and DELETE operations.

### Security

One good decision is that the OpenWeather key is not stored in the image. In
Docker Compose it is read from an environment variable, and in Kubernetes it
is read from a Secret. Also, only Nginx is exposed in the Compose setup, so
MongoDB is not directly exposed to the host.

There are still important security limitations:

- There is no authentication or authorisation, so the default `user1` is
  shared.
- Both C++ services return `Access-Control-Allow-Origin: *`. This is very
  open and would not be suitable for authenticated users.
- MongoDB has no username or password
- The city and user inputs have only simple validation.
- Public images and `mongo:latest` should be checked and pinned before a
  production deployment.

For this project, I kept the API key outside the source code and images,
which is an important basic protection. For a real version, I would make the
following improvements:

1. Add login and check that each user can only change their own favourites.
2. Remove the default user and pass the authenticated user identity to the
   favourites service.
3. Use specific allowed browser origins instead of `*`, or remove CORS
   headers when all requests go through Nginx.
4. Validate the input on the server and use proper HTTP methods for changes.
5. Enable MongoDB authentication, keep it on a private network, and use a
   database account with only the permissions it needs.
6. Add timeouts for the OpenWeather request and use a fixed version for each
   container image.
7. Add Kubernetes health checks, resource limits, backups, and monitoring as
   the application becomes more important.

For me, the main lesson was that Kubernetes does not replace the application
services. It gives a way to run and connect the containers, while the actual
weather and favourites behaviour still belongs to the C++ services.
