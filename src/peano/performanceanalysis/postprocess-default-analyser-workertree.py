from PIL import Image, ImageDraw, ImageFont

import csv
import sys

if len(sys.argv) != 2:
    print 'usage: python tobiDomain.py <filename>'
    sys.exit(0)

filename = sys.argv[1]
csvfile = open(filename, 'rb')
peanoreader_csv = csv.reader(csvfile, delimiter=",")

import pydot

graph = pydot.Dot(graph_type='graph')

for fork_line in peanoreader_csv:
  if fork_line[0] != '0':
    edge = pydot.Edge(fork_line[0], fork_line[1])
    graph.add_edge(edge)
    
if len(graph.get_edge_list()) == 0:
  graph.add_node(pydot.Node('1'))
  
graph.write_png(sys.argv[1]+'.png', prog='circo')

