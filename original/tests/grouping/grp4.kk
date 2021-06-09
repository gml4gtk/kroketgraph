<info	title="Example"
		date="25/06/2007" />

<node	id="n0"
		shape="circle"
		text="entry"
		text_style="bold"
		fill_color="#a9c3c3" />

<node	id="n1"
		text="step 1"
		text_color="blue" />

<node	id="n2"
		text="step 2"
		text_color="blue" />

<node	id="n3"
		text="exit"
		text_color="red"
		stroke_style="dots" />

<node	id="n4"
		text="alternative"
		text_color="red" />

<edge	id="e0"
		src_port="n0/output"
		src_style="open_dot"
		dest_port="n1/input"
		stroke_width="2" />

<edge	id="e1"
		src_port="n1/output"
		src_style="open_dot"
		dest_port="n2/input"
		stroke_width="2" />

<edge	id="e2"
		src_port="n2/output"
		src_style="open_dot"
		dest_port="n3/input"
		stroke_width="2" />

<edge	id="e3"
		src_port="n2/output"
		src_style="open_dot"
		dest_port="n4/input"
		stroke_width="2" />

<group id="g0"
		contents="[n0, n1]"
		shape="rectangle"
    text="group 0" />

<group id="g1"
		contents="[g0, n2]"
		shape="rectangle"
    text="group 1" />

<group id="g2"
		contents="[g1, n3]"
		shape="rectangle"
    text="group 2" />

