LATEX = latex
BIBTEX = bibtex
DVIPS = dvips
PS2PDF = ps2pdf

TARGET = thesis



###############################

all: $(TARGET)

draft:
	$(LATEX) draft.tex
	$(DVIPS) -f draft.dvi > draft.ps
	$(PS2PDF) draft.ps

thesis:
	$(LATEX) thesis.tex && $(BIBTEX) $(TARGET) && $(LATEX) thesis.tex
	$(DVIPS) -f thesis.dvi > thesis.ps


thesis.ps: dummy
	$(DVIPS) -f thesis.dvi > thesis.ps

thesis.pdf: dummy
	$(PS2PDF) thesis.ps

dummy: ;
###############################

clean:
	rm -f *~ *.log *.aux *.dvi *.ps *.pdf *.bbl *.blg *.toc

