/*
 * << H a r u --free pdf library >> -- EncodingListMb.cpp
 *
 * Copyright (c) 1999-2003 Takeshi Kanno <takeshi_kanno@est.hi-ho.ne.jp>
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.
 * It is provided "as is" without express or implied warranty.
 *
 */

#include "libharu.h"
#include "libharu_jpfonts.h"

/*
 * Character lists for Japanese EUC encoding.  
 */

void DrawGraph(PdfContents* canvas, unsigned char from);
void DrawFonts(PdfContents* canvas, unsigned char from);
void DrawPage(PdfDoc *doc, PdfOutlineItem *outline_item, unsigned int from);

static const int PAGE_WIDTH = 420;
static const int PAGE_HEIGHT = 230;
static const int CELL_WIDTH = 20;
static const int CELL_HEIGHT = 20;
static const int CELL_HEADER = 10;

int main()
{

    /* Create a PdfDoc object and start making a PDF document. */
    PdfDoc* doc = new PdfDoc();
    try {
        doc->NewDoc();
        doc->Catalog()->SetPageMode(PDF_USE_OUTLINES);

        /* Make helvetica font object. */
        PdfType1FontDef* fd1 = new PdfHelveticaFontDef();

        /* Add font with PdfStandardEncoding and name it "helvetica" */
        doc->AddType1Font(fd1, "helvetica");

        /* Add Gothic Font with euc encoding */
        PdfCMap* map_euc = new PdfCMap_EUC_H();
        doc->RegisterObject(map_euc);
        doc->AddType0Font(new PdfGothicFontDef(), "EUC Encoding", map_euc);
        
        /* Create outline root */
        PdfOutlineRoot *outline_root = doc->Outlines();
        PdfOutlineItem *outline_item = new PdfOutlineItem(outline_root);
        outline_item->SetTitle("EUC Encoding");
        outline_item->SetOpened(true);

        DrawPage(doc, outline_item, 0x8e);
        DrawPage(doc, outline_item, 0xa1);
        DrawPage(doc, outline_item, 0xa2);
        DrawPage(doc, outline_item, 0xa3);
        DrawPage(doc, outline_item, 0xa4);
        DrawPage(doc, outline_item, 0xa5);
        DrawPage(doc, outline_item, 0xa6);
        DrawPage(doc, outline_item, 0xa7);
        DrawPage(doc, outline_item, 0xa8);
        for (unsigned int a = 0xb0; a <= 0xf4; a++) 
            DrawPage(doc, outline_item, a);
        
        /* Save the document to the file. */
        doc->WriteToFile("EncodingListMb_EUC.pdf");

    } catch (PdfException& e) {
        fprintf(stderr, "%s\n", e.what());
        delete doc;
        return 1;
    }
    delete doc;
    return 0;
}

void DrawPage(PdfDoc *doc, PdfOutlineItem *outline_item, unsigned int from)
{
    char title[64];
    char caption[64];
    /* Add new page and set the size of the page */
    PdfPage* page = doc->AddPage();
    page->SetSize(PAGE_WIDTH, PAGE_HEIGHT);

    /* Get font name from FontManager object. */
    PdfFont* f = doc->FontMgr()->GetFont("EUC Encoding");
    sprintf(title, "%s  0x%02XA0 - 0x%02Xff", f->Name(), from, from);
    sprintf(caption, "0x%02XA0 - 0x%02Xff", from, from);

    /* Add the page to outline. */
    PdfOutlineItem *outline_item1 = new PdfOutlineItem(outline_item);
    outline_item1->SetTitle(caption);
    PdfDestination *dst = new PdfDestination(page);
    dst->SetXYZ(0, page->Height(), 1);
    outline_item1->SetDestination(dst);

    /* Draw graphs on the canvas. */
    PdfContents* canvas = page->Canvas();
    canvas->AddFilter(PDF_FILTER_DEFLATE);
    DrawGraph(canvas, from);

    canvas->BeginText();
    canvas->SetFontAndSize("helvetica", 10);
    canvas->MoveTextPos(40, PAGE_HEIGHT - 40);
    canvas->ShowText(title);
    canvas->EndText();

    /* Draw the gryphs of the font. */
    canvas->SetFontAndSize(f, 14);
    DrawFonts(canvas, from);
}

void DrawGraph(PdfContents* canvas, unsigned char from)
{
    /* Draw 16 X 15 cells */
    char buf[50];

    /* Draw vertical lines. */
    canvas->SetLineWidth(0.5);
    canvas->SetFontAndSize("helvetica", 8);
    for (int i = 0; i <= 17; i++) {
        int x = i * CELL_WIDTH + 40;

        canvas->MoveTo(x, PAGE_HEIGHT - 50);
        canvas->LineTo(x, 40);
        canvas->Stroke();

        if (i > 0 && i <= 16) {
            canvas->BeginText();
            canvas->MoveTextPos(x + 7, PAGE_HEIGHT - 65);
            snprintf(buf, 5, "%X", i - 1);
            canvas->ShowText(buf);
            canvas->EndText();
        }
    }

    /* Draw horizontal lines. */
    for (int j = 0; j <= 7; j++) {
       int y = j * CELL_HEIGHT + 40;

        canvas->MoveTo(40, y);
        canvas->LineTo(PAGE_WIDTH - 40, y);
        canvas->Stroke();

        if (j >= 0 && j < 6) {
            canvas->BeginText();
            canvas->MoveTextPos(47, y + 5);
            snprintf(buf, 5, "%X", 15 - j);
            canvas->ShowText(buf);
            canvas->EndText();
        }
    }
}

void DrawFonts(PdfContents* canvas, unsigned char from)
{
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 16; j++) {
            unsigned char buf[3];

            int y = PAGE_HEIGHT - 85 - (i * CELL_HEIGHT);
            int x = j * CELL_WIDTH + 70;

            if (from == 0) {
                buf[1] = 0x00;
                buf[0] = (i + 10) * 16 + j;
            } else {
                buf[2] = 0x00;
                buf[1] = (i + 10) * 16 + j;
                buf[0] = from;
            }
            double w = canvas->TextWidth((char*)buf);
            if (w > 0) {
                canvas->BeginText();
                canvas->MoveTextPos(x - w / 2, y);
                canvas->ShowText((char*)buf);
                canvas->EndText();
            }
        }
    }
}

