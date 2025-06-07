# TicketToRide
A bot Playing Ticket To Ride

## Functionality :
There is 3 ways to play the game, the manual play and 2 bots, 1 being random and the other using a strategy
The final bot is called firstbot because I originally planned to make others, but ended up just upgrading it.

### Strategy :
We start by analysing the board, and putting a weight to all roads. This weight is the sum of this road points and how much time this road appears in a shortest path. We always chooses objectives that have the most weight and score divided by the distance we need to complete them.
