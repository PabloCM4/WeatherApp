# WeatherApp

WeatherApp is a small weather application built as a set of C++ microservices.
It uses the [OpenWeather API](https://openweathermap.org/api) for weather data,
MongoDB for favourite cities, and an Nginx web front end. The services are
packaged with Docker and can also be deployed to Kubernetes.

## Requirements

- Docker and Docker Compose
- An [OpenWeather API key](https://home.openweathermap.org/users/sign_up)

## Run with Docker Compose

1. Create a file named `.env` in the project root:

   ```dotenv
   OPENWEATHER_API_KEY=your_openweather_api_key
   ```

   Keep this file private; the key is injected into the weather service at
   runtime and is not included in the Docker image.

2. Build and start the application:

   ```bash
   docker compose up --build
   ```

3. Open [http://localhost:3000](http://localhost:3000) in a browser. Select a
   city to view its weather, or add it to and remove it from the favourites
   list.

To stop the application, press `Ctrl+C`. To stop and remove the containers
without deleting the MongoDB data volume, run:

```bash
docker compose down
```

Favourite cities are stored in the `mongo_data` Docker volume and remain
available when the containers are recreated.

## Services

| Service | Purpose | Internal port |
| --- | --- | ---: |
| `web-service` | Nginx front end and reverse proxy | 80 |
| `weather-service` | Retrieves weather from OpenWeather | 8080 |
| `favourites-service` | Stores favourite cities | 8081 |
| `weather-mongo` | MongoDB database | 27017 |

Only the web service is exposed to the host by Compose (`localhost:3000`).
The reverse proxy routes browser requests to the internal services.

## Kubernetes (Minikube)

The repository includes [`WeatherApp.yaml`](WeatherApp.yaml), which defines
the web service, weather service, favourites service, and MongoDB StatefulSet.
The manifest uses the published images in Docker Hub, so start Minikube and
create the API key secret before applying it:

```bash
minikube start --driver=docker
kubectl create secret generic weather-secret \
  --from-literal=OPENWEATHER_API_KEY=your_openweather_api_key
kubectl apply -f WeatherApp.yaml
minikube service web-service
```

To remove the Kubernetes resources:

```bash
kubectl delete -f WeatherApp.yaml
```

## License

See [LICENSE](LICENSE).
