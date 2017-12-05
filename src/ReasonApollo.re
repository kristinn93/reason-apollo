module type CreationConfig = {let uri: string; let auth: string;};

module type ClientConfig = {type responseType; type variables;};

module Create = (CreationConfig: CreationConfig) => {
  Js.log("Test");
  let authHeadder: ApolloClient.authOptions = {"authorization": CreationConfig.auth};
  let contextLink =
    ApolloClient.setContext((_, authHeadder) => {"headers": "", "authorization": ""});
  let httpLinkOptions: ApolloClient.linkOptions = {"uri": CreationConfig.uri};
  let apolloClientOptions: ApolloClient.clientOptions = {
    "cache": ApolloClient.inMemoryCache(),
    "link": ApolloClient.httpLink(httpLinkOptions)
  };
  let apolloClient = ApolloClient.apolloClient(apolloClientOptions);
  module Query = (ClientConfig: ClientConfig) => {
    module CastApolloClient =
      ApolloClient.Cast(
        {
          type variables = ClientConfig.variables;
        }
      );
    let apolloClient = CastApolloClient.castClient(apolloClient);
    external cast : string => {. "data": ClientConfig.responseType, "loading": bool} = "%identity";
    type state =
      | Loading
      | Loaded(ClientConfig.responseType)
      | Failed(string);
    type action =
      | Result(string)
      | Error(string);
    let component = ReasonReact.reducerComponent("ReasonApollo");
    let make = (~query, ~variables=?, children) => {
      ...component,
      initialState: () => Loading,
      reducer: (action, _state) =>
        switch action {
        | Result(result) =>
          let typedResult = cast(result)##data;
          ReasonReact.Update(Loaded(typedResult))
        | Error(error) => ReasonReact.Update(Failed(error))
        },
      didMount: ({reduce}) => {
        let queryConfig =
          switch variables {
          | Some(variables) => CastApolloClient.getJSQueryConfig(~query, ~variables, ())
          | None => CastApolloClient.getJSQueryConfig(~query, ())
          };
        let _ =
          Js.Promise.(
            resolve(apolloClient##query(queryConfig))
            |> then_(
                 (value) => {
                   reduce(() => Result(value), ());
                   resolve()
                 }
               )
            |> catch(
                 (_value) => {
                   reduce(() => Error("an error happened"), ());
                   resolve()
                 }
               )
          );
        ReasonReact.NoUpdate
      },
      render: ({state}) => children[0](state)
    };
  };
};