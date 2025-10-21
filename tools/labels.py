import drawsvg as draw
from datetime import datetime
import argparse

WRITE_COLOR = '#000000'

schilder = {
1 : ["Blauer Bund",""],
2 : ["Roter Bund",""],
3 : ["Rolltore",""],
4 :	["Fahrradplatz","TV"],
5 :	["Ersatzteile","Container"],
6 :	["CNC Fräse",""],
7 :	["Lötplätze",""],
8 :	["Schubladen","Schrank"],
9 :	["Bohr-","maschine"],
10 : ["Hänge-","schränke"],
11 : ["Hänge-","schränke (F)"],
12 : ["Drechsel-","bank"],
13 : ["Bandsäge",""],
14 : ["Handtuch-","spender"],
15 : ["Kappsäge",""],
16 : ["Gefahrstoffe",""],
17 : ["Schrauben-","schrank"],
18 : ["Keller","3D Druck"],
19 : ["TrudeTrailer",""]
}

D = 4.4
L = 80.0
B = 28.0
A = 2.8
LINE = 1.0
OFFSET_TEXT_X = 6.5
OFFSET_TEXT_Y = 1


def calc_offset(i,all_off = (10,10)):
    global L,B
    
    col = i % 3
    row = i // 3
    
    off_x =  col * (L + 20) + all_off[0]
    off_y =  row * (B + 20) + all_off[1]

    return (off_x,off_y)
    

def make_one(dr,line1,line2,number,offset,font_size=8):
    global D,L,B,LINE,OFFSET_TEXT_X,OFFSET_TEXT_Y
    (x0,y0) = offset
    if line2 != "":
        t = [line1,line2]
        dr.append(draw.Text(t, font_size,
                        x0 + L/2 + OFFSET_TEXT_X,
                        y0 + B/2 - OFFSET_TEXT_Y,
                       fill=WRITE_COLOR,
                       text_anchor='middle',
                       font_family='Arial',
                       font_weight='bold'))
    else:
        dr.append(draw.Text(line1, font_size,
                        x0 + L/2 + OFFSET_TEXT_X,
                        y0 + B/2 - OFFSET_TEXT_Y + font_size/2,
                       fill=WRITE_COLOR,
                       text_anchor='middle',
                       font_family='Arial',
                       font_weight='bold'))
        
    dr.append(draw.Text(str(number), 14,
                    x0 - B/2,
                    y0 + 20,
                    fill='none',
                    stroke=WRITE_COLOR,
                    text_anchor='middle',
                    font_family='Arial',
                    transform=f"rotate(270.0,{x0},{y0})"))

    dr.append(draw.Rectangle(x0, y0, L, B,
                            fill='none', 
                            stroke=WRITE_COLOR,
                            stroke_width=LINE))
    
    dr.append(draw.Circle(x0 + L - A - D / 2 ,
                         y0 + B / 2,
                         D / 2,
                         fill='none', 
                         stroke=WRITE_COLOR,
                         stroke_width=LINE))
    
    dr.append(draw.Circle(x0 + A + D / 2 ,
                         y0 + B / 2,
                         D / 2,
                         fill='none', 
                         stroke=WRITE_COLOR,
                         stroke_width=LINE))
d = draw.Drawing(400, 400, origin=(0, 0))

for i in range(19):
    offset = calc_offset(i)
    make_one(d,schilder[i+1][0],schilder[i+1][1],i+1,offset)
d.set_pixel_scale(3.7795)
d.save_svg("schilder.svg")