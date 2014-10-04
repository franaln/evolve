MAIN = slides

OBJS = $(MAIN).toc $(MAIN).aux $(MAIN).out $(MAIN).toc $(MAIN).log $(MAIN).snm $(MAIN).vrb $(MAIN).nav

all: pdf
	rm -f $(OBJS)
	@echo "$@ done"

pdf: $(MAIN).tex
	@echo "Latexeando $(MAIN).tex"
	latex $(MAIN).tex
	latex $(MAIN).tex
	@dvips $(MAIN).dvi
	@ps2pdf $(MAIN).ps


clean:
	@rm -rf $(MAIN).dvi
	@rm -rf $(MAIN).ps
	@rm -rf $(MAIN).pdf
