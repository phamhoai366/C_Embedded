# # Recursive variable. This will print "later" below
# one = one ${later_variable}
# # Simply expanded variable. This will not print "later" below
# two := two ${later_variable}

# later_variable = later

# all: 
# 	echo $(one)
# 	echo $(two)

# one = hello
# one ?= will not be set
# two ?= will be set

# all: 
# 	echo $(one)
# 	echo $(two)


with_spaces = hello    # with_spaces has many spaces after "hello"
after = $(with_spaces)there

nullstring =
space = $(nullstring) # Make a variable with a single space.

all: 
	echo "$(after)"
	echo start"$(space)"end