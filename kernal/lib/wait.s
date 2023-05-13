.export _wait, _stop

.segment "CODE"

.proc _wait : near
	cli
	wai
	rts
.endproc

.proc _stop : near
	stp
.endproc
