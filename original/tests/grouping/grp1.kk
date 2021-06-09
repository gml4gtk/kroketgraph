<info	title="Example"
		date="25/06/2007" />

<node	id="n0"
		shape="circle"
		text="entry"
		text_style="bold"
		fill_color="#a9c3c3" />

<node	id="n1"
		text="exit"
		text_color="red"
		stroke_style="dots" />

<edge	id="e0"
		src_port="n0/output"
		src_style="open_dot"
		dest_port="n1/input"
		stroke_width="2" />

<group id="g0"
		contents="[n0, n1]"
		shape="rectangle"
    text="group 0" />
