Note: This is an excavation of my 2003 master's thesis and project, preserved in its original state.

# About

Exalt (An Experimental XML Archiving Library/Toolkit) is a C++ library for compression of XML documents. The library is an academic project, and is intended to serve as a platform for experiments rather than as a basis for industrial-strength applications. The main focus of the implementation was not on speed and memory sparing issues. The goal was to create a framework for XML data compression, which would be easy to use and to extend. Please keep this in mind.

Exalt uses [Expat](https://libexpat.github.io/), the excellent XML parser written by James Clark.

Exalt is free software, licensed under the GNU General Public License, Version 2. However, some parts of the sources (the arithmetic coding routines, to be exact) originate from [Alistair Moffat](https://people.eng.unimelb.edu.au/ammoffat/arith_coder/), who made them only available for academic purposes.

## XML and Compression

Because XML is an extremely verbose language, the documents in XML are usually much larger than in other representations. This makes XML a candidate for data compression. The common approach is to apply existing text compressors such as `gzip`. However, the performace of these compressors is not always as good as one would expect - although it is obvious that XML contains a lot of redundancy. The problem is that this redundancy is hidden in the XML structure, and it is hard for standard text compressors to discover it and to utilize it in the compression. A more sophisticated approach is therefore needed.

In these days, there do exist various tools for XML data compression - [XMill](https://citeseerx.ist.psu.edu/document?repid=rep1&doi=141e8528f0d3cdcf5dcbfe3ba5be5af494832c09), XMLZip and [XMLPPM](https://homepages.inf.ed.ac.uk/jcheney/programs/xmlppm/xmlppm.html) among others. However, there are still many ways to be explored on the field of XML data compression. These tools adapt existing text compressors to XML very often. XMill, for example, scatters the data into a set of data containers and uses gzip to compress them. XMLPPM uses several contexts for encoding XML entities (elements, attributes, etc.) and uses an adapted version of PPM.

Exalt takes a different approach. Since XML has a context-free nature, syntactic compression is applied. Exalt implements a technique based on so called [grammar-based codes](https://citeseerx.ist.psu.edu/doc_view/pid/15c3bdc536d180a7b8931e3453726aba2f9ca0a2) introduced recently by J. C. Kieffer and E.-H. Yang. Apart from other reasons, it was also an interesting test on how it performed.

It is clear that syntactic compression by itself is not enough for efficient XML compression. Some kind of modeling that would help to discover the redundancy is desirable. There are two modeling strategies implemented in Exalt: the simple modeling, and the adaptive modeling.

The policy of the simple modeling strategy is very simple. The input XML data is transformed such that the appearances of the known elements and attributes as well as the ends of the elements are represented by numeric tokens to save space. The transformed data is then passed to the syntactic compression. This approach brings nothing new to the XML compression, since a similar technique is used in XMill, for instance. However, the simple modeling acts as a good foundation for further experiments.

The adaptive modeling strategy, roughly speaking, tries to do some probabilistic modeling of the elements. During parsing the input XML data, models for individual elements are being built to reflect their structural and probabilistic chararacteristics. The models are then used to predict the structure of the following data, and to improve the compression performance.

Interesting to note is that the adaptive modeling strategy is not always better than the simple strategy. In fact, it performs the same on average. However, the adaptive modeling yields a faster run, since fewer symbols have to be processed by the syntactic compressor.

An unique feature of the Exalt library is that it can act as an ordinary SAX parser. It can read compressed XML data and emit SAX events to the application. The SAX interface of Exalt is nearly identical to the interface of the Expat XML parser.

For more details on the theoretical/implementation background of Exalt, please refer to the Exalt home page.

# Documentation

You can see the Exalt [reference documentation](./doc/reference.html). The source of the master's thesis is available [here](./thesis/).
