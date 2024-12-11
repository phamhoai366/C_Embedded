hey: one two three		

	# $@ is the automatic variable for target name		| hey
	echo $@

	# $< is the automatic variable for first dependency	| one
	echo $<

	# $^ is the automatic variable for all dependencies	| one two three
	echo $^

	# $? is the automatic variable for all dependencies that are newer than the target	| one two three
	echo $?

one: 
	touch one

two:
	touch two

three:
	touch three

clean:
	rm -f hey one two three