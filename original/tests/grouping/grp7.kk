<info	title="Example"
		date="25/06/2007" />

<node	id="n0_1"
		text="n0_1" />

<node	id="n0_2"
		text="n0_2" />

<node	id="n1_1"
		text="n1_1" />

<node	id="n1_2"
		text="n1_2" />

<edge	id="e0"
		src_port="n0_1"
		dest_port="n0_2" />

<edge	id="e1"
		src_port="n1_1"
		dest_port="n1_2" />

<edge	id="e0"
		src_port="n0_2"
		dest_port="n1_2" />

<group id="g0"
		contents="[n0_1, n0_2]"
    text="group 0" />

<group id="g1"
		contents="[n1_1, n1_2]"
    text="group 1" />

<group id="g2"
		contents="[g0, g1]"
    text="group 2" />
