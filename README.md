# DeKUF - Decentralizing KUserFeedback

A system for gathering more meaningful data in a more privacy-friendly way.

For more background, see [the wiki][1].

## Components

DeKUF consists of two separate components, which are - for the time being - in
the same repository, this one.

1. [client](client)
2. [server](server)

## Testing

Testing the whole system is currently only semi automated, but we plan to add
fully automated integration tests over time. For now, you'll need to follow
these steps:

### 1. Launch the services

The easiest way to do that is via [Docker
Compose](https://docs.docker.com/compose/):

    docker compose up

This will build and spawn the server, and a sufficient amount of clients.

**Note:** The amount of clients needs to be at least `Group size * Group count`,
at the time of writing that's **4**. If the number is now higher, you might need
to adjust it in [docker-compose.yaml](docker-compose.yaml).

### 2. Set up the server

This part is already automated:

    ./demo server-init

But in case you want to do it manually:

1. Go to [http://localhost:8080/admin/](http://localhost:8080/admin/).
2. Log in as _admin_ with password _dekuf_ - at least these are the credentials
   set at the time of writing.
3. Add a new _Survey_.
    1. Set _Name_ to _Test_.
    2. Add a new _Commissioner_ called _KDE_.
    3. Set both _Group size_ and _Group count_ to `2`.
    4. Add a new _Data point_ called _Test_ with _Key_ _test_ and _Type_
       _Integer_.
    5. Set the query's _Cohorts_ to `["1", "2", "3"]`. Leave _Discrete_ checked.
    6. Save it.

### 3. Initialise the clients with data

This part is already automated:

    ./demo client-init

It should set the datapoint _test_ for each client, with values `1`, `2`, `2`
and `3` respectively, at least at the time of writing.

### 4. Verify the results on the server

Wait a few minutes for the clients to subscribe to the survey, finish and submit
the aggregation.

Then, in the admin interface, check _Survey responses_ and verify that one has
been created. The value should be: `{"1": 1, "2": 2, "3": 1}`.

### Debugging

If the expected results don't show up on the server, you can check each step of
the protocol on the server (in addition to watching the client for error
messages). The steps are, roughly:

1. Each client does a _Survey signup_, which should show up in the admin as soon
   as they do.
2. _Aggregation groups_ are formed once enough clients have signed up, and a
   delegate (the aggregating client) will be chosen. In the admin, you can see
   the ID of the delegate.
3. The non-delegate clients send messages intended for the delegate through the
   server. You should see these (albeit encrypted) under _Client to delegate
   messages_.
4. Once the delegate has received all messages, it _should_ send a _Survey
   response_.

Please note that the actual naming and some of the details of the protocol may
have changed since this was written.

### Other things to test

In addition to _discrete_ queries, we can also do continuous queries, using
interval notation. A test case for this is not documented yet.

[1]: https://gitlab.com/privact/dekuf/-/wikis/home
