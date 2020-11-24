#main.tex et biblio.bib

.PHONY : noBib, clean
noBib : 
	pdflatex main.tex


.PHONY : withBib
withBib : 
	pdflatex main.tex
	bibtex main.aux
	pdflatex main.tex
	pdflatex main.tex

clean :
	rm *.out *.log *.aux