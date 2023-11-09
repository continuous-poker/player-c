FROM alpine as builder
RUN apk add --no-cache build-base
WORKDIR /app
COPY . .
RUN make player-c

FROM alpine as server
WORKDIR /app
COPY --from=builder /app/player-c /app/server
EXPOSE 8000
CMD ["/app/server"]