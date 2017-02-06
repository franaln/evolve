#! /usr/bin/env python2.7

import sys
from array import array
import ROOT

def get_color(color):
    if isinstance(color, str):
        if color.startswith('#'):
            color = ROOT.TColor.GetColor(color)
        else:
            if '+' in color:
                col, n = color.split('+')
                color = getattr(ROOT, color)
                color += int(n)
            elif '-' in color:
                col, n = color.split('-')
                color = getattr(ROOT, color)
                color -= int(n)
            else:
                color = getattr(ROOT, color)

    return color

def set_color(obj, color):
    color = get_color(color)
    obj.SetLineColor(color)
    obj.SetMarkerColor(color)

def main():

    if len(sys.argv) < 2:
        print "usage: analyselog.py [logfile]"
        return 1

    logfile = sys.argv[1]

    generations = []

    with open(logfile) as log:

        lines = log.read().split('\n')

        gen_idx = -1
        for i, line in enumerate(lines):

            if not line:
                continue

            if line.startswith('--- Generation'):
                gen_idx += 1
                generations.append([])

            elif not line.startswith('---'):

                tmp = line.split('|')

                indv = dict()
                indv['sig'] = float(tmp[-1].strip().replace('Z = ', ''))
                indv['cuts'] = [ float(x.strip()) for x in tmp[:-1] ]

                generations[gen_idx].append(indv)

            else:
                continue

    ngen = len(generations)
    pop_size = len(generations[0])

    # Plot significance vs generation
    sig_x     = array('d', [i for i in xrange(ngen)])
    sig_best  = array('d')
    sig_mean  = array('d')
    sig_worst = array('d')

    for gen in generations:

        sig_best.append(gen[0]['sig'])
        sig_worst.append(gen[-1]['sig'])

        sig_sum = sum([ indv['sig'] for indv in gen ])

        sig_mean.append(sig_sum/pop_size)


    g_best  = ROOT.TGraph(ngen, sig_x, sig_best)
    g_mean  = ROOT.TGraph(ngen, sig_x, sig_mean)
    g_worst = ROOT.TGraph(ngen, sig_x, sig_worst)

    set_color(g_best, '#2f8a24')
    set_color(g_mean, '#3182ba')
    set_color(g_worst, '#c73444')

    g_best.SetLineWidth(2)
    g_mean.SetLineWidth(2)
    g_worst.SetLineWidth(2)

    g_best.SetFillColor(0)
    g_mean.SetFillColor(0)
    g_worst.SetFillColor(0)

    g_best.SetMarkerStyle(20)
    g_mean.SetMarkerStyle(20)
    g_worst.SetMarkerStyle(20)

    g_best.SetTitle('')
    g_best.GetYaxis().SetRangeUser(0,50)
    g_best.GetYaxis().SetTitle('Significance')
    g_best.GetXaxis().SetTitle('Generation')

    c1 = ROOT.TCanvas()

    g_best.Draw('PAL')
    g_mean.Draw('PL')
    #g_worst.Draw('PL')

    leg = ROOT.TLegend(0.7, 0.7, 0.85, 0.85)
    leg.SetBorderSize(0)
    leg.AddEntry(g_best, 'Best')
    leg.AddEntry(g_mean, 'Mean')
    #leg.AddEntry(g_worst, 'Worst')
    leg.Draw()

    c1.Print('sig_vs_generation.pdf')




if __name__ == '__main__':
    main()
