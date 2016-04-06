#!/usr/bin/env python

import os,sys

class class_A( object ): #< Note the inheritance of `object`. 
	def __init__( self , name):
		self.name = name
		print 'Class A'

class class_B( class_A ):
	def __init__( self , name , email):
		print 'Class B'
		super( class_B, self ).__init__(name)
		self.email = email

class_B("Bob Johnson","sss@email.com")

