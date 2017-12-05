module type CreationConfig = {let uri: string; let auth: string;};

module type ClientConfig = {type responseType; type variables;};

module Create = (CreationConfig: CreationConfig) => {
  Js.log("Test");
  let authHeadder: ApolloClient.authOptions = {"authorization": CreationConfig.auth};


  let constructHeaders = [%bs.raw {|function(a, { headers }) {
    return {
      headers: {
        ...headers,
        authorization: token ? `Bearer ${CreationConfig.auth}` : null,
      }
    }
  }|}];


  let contextLink =
    ApolloClient.setContext(constructHeaders);
  let httpLinkOptions: ApolloClient.linkOptions = {"uri": CreationConfig.uri};
  let httpLink = ApolloClient.httpLink(httpLinkOptions);
  let apolloClientOptions: ApolloClient.clientOptions = {
    "cache": ApolloClient.inMemoryCache(),
    "link": contextLink##concat(httpLink)
  };
  let apolloClient = ApolloClient.apolloClient(apolloClientOptions);
  /* contextLink.concat("fooo"); */
  /* contextLink.concat("foo"); */
  /* contextLink##concat("foo"); */
};