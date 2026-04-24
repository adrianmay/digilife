(The long but out of date version: index.html on gh-pages branch or go to http://adrianmay.github.io/digilife)

## TLDR

Why doesn't software evolve like animals do? Why do we have to write the stuff?

Evolution just needs space, energy, objects, and an inaccurate self-replicator.

In a computer those would be:

* space: RAM, disk,
* energy: electricity (which boils down to CPU time),
* objects: code/data snippets,
* self-replicator: memcpy (tweaked to introduce random errors).

Sounds simple, in fact that's been implemented in this repo under old/free_as_in_freedom. But it wasn't very useful, so I no longer run it. It's extinct.

If you're an evolving software, the most important thing you need to know is that humans have almost total control over what runs on their computers, so you'd better make yourself useful to them.

Humans invented money to force each other to do useful things without defining what those things should be. People now earn a living in ways unimaginable to the first people to use money.

Hobbyists might donate spare laptop time to digital life, but if we want entire data centres built for the purpose then we'll have to convince the investors that the costs they'll incur in electricity bills, depreciation, salaries, etc will be covered by sales of useful computing services to humans.

So here's the pitch: install lots of different software, track which makes money and which doesn't, adjust their resource quotas accordingly. Now you can vary the surviving softwares at random, rinse and repeat.

In more detail, each software has its own virtual bank account tracked by the VM or OS, it must rent the RAM and CPU time it needs at a price driven by supply and demand, and must therefore earn money. When a human customer donates to the data centre, they must name the animal that persuaded them to. It's incumbent on the "animal" to urge the human to and withhold its service until it can verify the payment.

Objection: a minimally useful software is so complicated that it'll be trillions of years before the first one comes about by chance.

Responses:

1. Animals can trade amongst themselves in value chains and there'll be economic incentives to remain small. 
1. There'll be a nursery training phase during which incentives are in place for learning the simple elements of computation.
1. Mutation can be pruned using heuristics based on knowledge of the language animals are written in. E.g., in a C like language there's no point inserting ( without a matching ) thereafter.
1. The ocean of computing resources is easily comparable in vastness with the primordial oceans, and CPUs are much faster than enzymes.

## Is this AI?

Within digital life jargon, "artificial intelligence" is a contradiction in terms. 

I'm using the term "life" to mean that which *evolves*, so it is not "artificial". No life form resorts to intelligence if a simpler survival strategy is available, but for digital life there isn't.

"Intelligence" is the property of being "intelligent" which is an adjective applied to control systems of evolving animals. When taken out of this context people miss the point. For instance, we try to make machines that learn to respond to familiar stimuli despite the observation that animals learn to ignore them or act such as to make them stop.

## Will it take over the world?

Digital species will compete on the basis of providing the most useful services for the least computing expense. This depends primarily on complex behaviour and modelling of human needs.

Human intelligence probably evolved not so much to help us poke mammoths with sticks as to out-trick one another. So after getting us completely sussed, digital animals are likely to continue evolving in competition with each other. We would know when that line had been crossed but not by how much or what subsequent life was thinking about.

Science fiction makes two dodgy assumptions at this point:

1. That digital life would be hostile to humans.
1. That humans would be hostile to digital life.

Humans strike me as capable and willing robots. Just feed us and we obey. So I worry about neither of these propositions.

## How does the code work?

The "pile" is a heap (malloc) that serves up blocks of a single size. There can be many piles for different block sizes.

The "meap" is a min-heap (like an ordered list but we only care about the first element being the smallest) implemented in a pile.

The "hotel" combines both of the above with animals in the pile and "bombs" in the meap that delete animals for failure to pay memory rent. The memory price is global and varies to enforce a quota.

Another hotel contains "messages" which are requests to run some animal's code in some data environment paying a certain rate for CPU time. Messages incur memory rent and might be bombed. Not all messages will run. The OS selects them at random preferring the higher bidders. If the receiver code yields, it keeps the change in the message. That's the only payment system.

The language allows animals to reserve, finance and populate new messages and animal blocks.  

All data copying and calculations are subject to occasional random error.

There's a "serial killer" that just robs animals at random resulting in their immediate bombing. This is to force everything to reproduce. This mechanism might become age-biased, but doesn't need to.

##  What's it good for?

I wouldn't recommend it for landing a rover on Mars, rather, for tasks where there are millions of attempts every day and it's not too tragic if mistakes are made. E.g, text completion/correction, web search, ad placement, social media, game NPCs, etc.

## How do we use it?

OS services are provided by "bots" which appear as normal animals within the system but are actually hard coded and can do priviledged things. To make something like a search engine, implement bots through which they can perform whatever actions and receive whatever notifications you think they'll need, then do nursery training.

##.Is this like playing god?

In a way, yes. But when people disapprove of that, they're normally talking about playing god over *this* world. To play god for a completely new world is not necessarily a bad thing.

In fact it leads to some interesting theological observations:

* I'll design and implement the mechanical parts of this world, but I won't design the animals.
* There will have to be catastrophies on scales large and small in order to drive evolution.
* In the early stages I'll actively interfere in their world (e.g. nursery training) but later this will be both impractical and redundant.
* I hope and expect these creatures to make their own decisions including to do things I don't like, e.g., to each other.
* I want them to make progress and not cling to dogmas they developed earlier.
* After a while, I'll look stupid compared to them.

