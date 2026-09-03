// Mod RPC ids must stay clear of the vanilla range - always start at 10000 or above.
enum CFCloud_Bridge_RPC
{
	INVALID = 10000,
	SYNC_SETTINGS,      // Server -> Client
	REQUEST_SYNC,       // Client -> Server
	COUNT
};
