open ReasonApolloTypes;

type variableTypeToBeDefined;

type queryObj = {. "query": queryString, "variables": variableTypeToBeDefined};

type generatedApolloClient = {. "query": [@bs.meth] (queryObj => string)};

type clientOptions = {. "cache": unit, "link": unit};

type linkOptions = {. "uri": string};

type authOptions = {. "authorization": string};

[@bs.module "apollo-client"] [@bs.new]
external apolloClient : clientOptions => generatedApolloClient =
  "ApolloClient";

[@bs.module "apollo-link-http"] [@bs.new] external httpLink : linkOptions => 'a = "HttpLink";

[@bs.module "apollo-link-context"] external setContext : authOptions => 'a = "";

[@bs.module "apollo-cache-inmemory"] [@bs.new] external inMemoryCache : unit => 'a =
  "InMemoryCache";

module type ApolloClientCast = {type variables;};

/* Cast the apolloClient, with the known variable type when called */
module Cast = (ApolloClientCast: ApolloClientCast) => {
  type queryObj = {. "query": queryString, "variables": ApolloClientCast.variables};
  external castClient : generatedApolloClient => {. "query": [@bs.meth] (queryObj => string)} =
    "%identity";
  [@bs.obj]
  external getJSQueryConfig :
    (~query: queryString, ~variables: ApolloClientCast.variables=?, unit) => queryObj =
    "";
};