// Quotes

var quotes = new Array ();

// to do: define array externally?

quotes.push({quote: "Survival is the ability to swim in strange water.",  author: "Frank Herbert", source: "Dune"});
quotes.push({quote: "Don't Panic.", author:  "Douglas Adams", source: "The Hitchhiker's Guide to the Galaxy"});
quotes.push({quote: "The fourth dimension is weird.", author:"Matt Parker", source: "Things to make and do in the fourth dimension"});
quotes.push({quote:"There are some ideas so absurd that only an intellectual could believe them.", author:"George Orwell"});
quotes.push({quote: "Facts are stubborn things, but statistics are pliable.", author: "Mark Twain"});

var qi = Math.floor(quotes.length*Math.random())
var quotefield = document.getElementById("quote-field");
var quoteauthor = document.getElementById("quote-author");
var selQuote = quotes[qi];
quotefield.innerHTML =  "<q>" + selQuote.quote + "</q>";
quoteauthor.innerHTML = "<p>" + selQuote.author + "</p>";

if (typeof(selQuote.source) !== 'undefined' ) {
    quotefield.title  = "In: " + selQuote.source;
    quoteauthor.title = quotefield.title;
}

