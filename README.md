# DeKUF - Decentralizing KUserFeedback

A system for gathering more meaningful data in a more privacy-friendly way.

For more background, see [the wiki][1].

## Components

DeKUF consists of two separate components, which are - for the time being - in
the same repository, this one.

1. [client](client)
2. [server](server)

## Testing

Running an integration test is currently a fairly manual process which we will
automate over time. These are the steps you'll need to follow.

### 1. Set up the server

1. Launch the [server](server)
2. Go to [http://localhost:8000/admin/](http://localhost:8000/admin/)
3. Add a new _Survey_
    1. Set _Name_ to _Test_
    2. Add a new _Commissioner_ called _KDE_
    3. Set both _Group size_ and _Group count_ to `2`
    4. Add a _Query_ with _Data key_ _test_ and _Cohorts_ `["1", "2", "3"]`.
       Leave _Discrete_ checked

### 2. Launch the clients

You will need to spawn `Group size * Group count` clients, so in our case,
**4**.

**Please note:** Currently, this is a manual process, and spawning multiple
clients on the same machine isn't even possible. Yet, the clients need to be on
the same machine as the server, since they hard code the server URL as
_localhost_. Consequently, this is not currently something you _can_ do, but a
note of what you _should_ do. To push a _data point_ into a client, you can use
the `submit_data_point.py` script.

1. Spawn the first client, and push the data point `test` with value `1`
2. Spawn the second client, and push the data point `test` with value `2`
3. Spawn the third client, and push the data point `test` with value `2`
4. Spawn the fourth client, and push the data point `test` with value `3`

Wait a few minutes for the clients to subscribe to the survey, finish and submit
the aggregation.

### 3. Verify the results on the server

In the admin interface, check _Survey responses_ and verify that one has been
created. The value should be: `{"1": 1, "2": 2, "3": 1}`.

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
