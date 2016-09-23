args <- commandArgs(trailingOnly = TRUE)
library(Hmisc)
x<-read.table("tmp",header=T)
title<-paste("CNT:",args[1],"RTT:",args[2],"CSZ:",args[3],sep=" ")
dat<-data.frame(cond=rep(c("Stackoverflow","go","guardian"),each=3),xvar=x$x1,yvar=x$x2)
pdf(args[4],width=6,height=4,paper='special')
ggplot(dat, aes(x=xvar, y=yvar/1000, color=cond))  +geom_point(size=2)+geom_line(aes())+ 
	theme_bw()+labs(x="PLR [%]", y="PLT [s]",linetype='custom title',fill="")+ ggtitle(title) + 
	theme(legend.title=element_blank(),text = element_text(size=20), legend.position = c(0.8, 0.8),plot.title = element_text(lineheight=.8, face="bold"))
dev.off()
