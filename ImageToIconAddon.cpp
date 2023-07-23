/*
 * Copyright (C) 2002-2003  CKJ Vincent Cedric
 * Copyright (C) 2011  Chris Roberts
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include <Application.h>
#include <Alert.h>
#include <TrackerAddOn.h>
#include <TranslationUtils.h>
#include <Path.h>
#include <Bitmap.h>
#include <NodeInfo.h>


status_t ConvertToIcon(BEntry* entry);
void ScaleToFit(uint16& width, uint16& height);
void FillTransparent(BBitmap* icon);
void ScaleBitmapToIcon(BNodeInfo* nodeInfo, BBitmap* bitmap, icon_size which, uint16 width, uint16 height);


void
process_refs(entry_ref dir_ref, BMessage* msg, void* reserved)
{
	entry_ref fileRef;
	BEntry entry;
	int32 index = 0;

	while (msg->FindRef("refs", index, &fileRef) == B_OK) {
		entry.SetTo(&fileRef);
		if (entry.InitCheck() == B_OK)
			ConvertToIcon(&entry);

		index++;
	}
}


int
main(int argc, char** argv)
{
	BApplication errorApp("application/x-vnd.cpr.ImageToIcon");

	(new BAlert("", "This is a Tracker Add-On.\nYou must copy it to /boot/home/config/add-ons/Tracker and use the Tracker Add-Ons menu to launch it.", "Ok"))->Go();

	return 0;
}


status_t
ConvertToIcon(BEntry* entry)
{
	BBitmap* bitmap = NULL;
	entry_ref fileRef;

	if (entry->GetRef(&fileRef) != B_OK)
		return B_ERROR;

	bitmap = BTranslationUtils::GetBitmap(&fileRef);
	if (bitmap == NULL)
		return B_ERROR;

	BNode node(entry);
	BNodeInfo nodeInfo(&node);

	if (nodeInfo.InitCheck() != B_OK)
		return B_ERROR;

	uint16 size_x, size_y;

	size_x = (uint16)((bitmap->Bounds()).Width());
	size_y = (uint16)((bitmap->Bounds()).Height());
	ScaleToFit(size_x, size_y);

	ScaleBitmapToIcon(&nodeInfo, bitmap, B_LARGE_ICON, size_x, size_y);

	ScaleBitmapToIcon(&nodeInfo, bitmap, B_MINI_ICON, size_x / 2, size_y / 2);

	delete bitmap;

	return B_OK;
}


void
ScaleToFit(uint16& width, uint16& height)
{
	float step = 0;

	if (width > height) {
		step = width / 31;
		width = 31;
		height = (uint16)(height / step);
	} else {
		step = height / 31;
		height = 31;
		width = (uint16)(width / step);
	}
}


void
FillTransparent(BBitmap* icon)
{
	int32 size = (int32)(((icon->Bounds()).Width() + 1) * ((icon->Bounds()).Height() + 1));
	for (int32 pixel = 0; pixel < size; pixel++)
		*((int8*)(icon->Bits()) + pixel) = B_TRANSPARENT_MAGIC_CMAP8;
}


void
ScaleBitmapToIcon(BNodeInfo* nodeInfo, BBitmap* bitmap, icon_size which, uint16 width, uint16 height)
{
	BView* offView = NULL;
	BBitmap* icon = NULL;

	offView = new BView(BRect(0, 0, which - 1, which - 1), "off-view", B_FOLLOW_NONE, B_WILL_DRAW);
	icon = new BBitmap(BRect(0, 0, which - 1, which - 1), B_CMAP8, true, true);

	FillTransparent(icon);

	icon->Lock();
	icon->AddChild(offView);
	offView->DrawBitmapAsync(bitmap, BRect(0, 0, width, height));
	offView->Sync();
	icon->Unlock();

	nodeInfo->SetIcon(icon, which);

	icon->Lock();
	icon->RemoveChild(offView);
	icon->Unlock();

	delete offView;
	delete icon;
}
