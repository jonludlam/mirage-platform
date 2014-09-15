module IO : sig
  type 'a t = 'a Lwt.t
  val return: 'a -> 'a t
  val ( >>= ): 'a t -> ('a -> 'b t) -> 'b t

  type backend = [ `xen | `unix ]
  val backend : backend

  type channel
  val create: unit -> channel t
  val destroy: channel -> unit t
  val read: channel -> string -> int -> int -> int t
  val write: channel -> string -> int -> int -> unit t
end
