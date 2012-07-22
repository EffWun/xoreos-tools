/* libeos 0.0.1
 * Copyright (c) 2005 Sven Hesse (DrMcCoy)
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include <eos.h>

Dialog::Entry::Entry()
{
	anim = delay = questentry = 0;
	pc = false;
}

void Dialog::Entry::del(void)
{
	std::vector<Dialog::Entry::Reply>::iterator itr;
	std::vector<class Dialog::Entry *>::iterator itp;
	Entry *ce = NULL;

	for(itr=replies.begin();itr!=replies.end();++itr)
		if((ce = ((*itr).child)))
		{
			if(!(*itr).link) ce->del();
			else // Erase child's references to this entry
				for(itp=ce->parents.begin();itp!=ce->parents.end();++itp)
					if(*itp == this) *itp = NULL;
		}

	for(itp=parents.begin();itp!=parents.end();++itp)
		if(*itp) // Erase parents' references to this entry
			for(itr=(*itp)->replies.begin();itr!=(*itp)->replies.end();++itr)
				if(itr->child == this) itr->child = NULL;

	delete this;
}

// Delete NULL-references for good
void Dialog::Entry::tidy(void)
{
	std::vector<Dialog::Entry::Reply>::iterator itr;

	itr = replies.begin();
	while(itr != replies.end())
	{
		if(!itr->child)
		{
			replies.erase(itr);
			continue;
		}
		if(!itr->link) itr->child->tidy();
		++itr;
	}
}

bool Dialog::Entry::isWSp(char c)
{
	if((c == ' ') || (c == '\t') || (c == '\v') || (c == '\r') || (c == '\n') ||
		 (c == '\f'))
		return true;
	return false;
}

uint32 Dialog::Entry::countWords(void)
{
	std::vector<Dialog::Entry::Reply>::iterator itr;
	uint32 n=0;

	for(uint32 i=0;i<NWN_LANG_MAX;i++)
		n += countWords(text.str[i]);
	for(itr=replies.begin();itr!=replies.end();++itr)
		if(!itr->link && itr->child) n += itr->child->countWords();
	return n;
}

uint32 Dialog::Entry::countWords(std::string &str)
{
	uint32 n=0;
	bool white=true;

	for(uint32 i=0;i<str.length();i++)
	{
		if(white && !isWSp(str[i]))
		{
			white = false;
			n++;
		}
		else if(!white && isWSp(str[i]))
			white = true;
	}
	return n;
}

void Dialog::Entry::add(Entry *e, std::string active, bool link, std::string lcomment)
{
	uint32 n;

	replies.resize((n=replies.size())+1);
	replies[n].child = e;
	replies[n].active = active;
	replies[n].link = link;
	replies[n].linkcomment = lcomment;
	e->pc = !pc;
}

Dialog::Dialog()
{
	preventzoom = false;
	delayentry = delayreply = wordcount = 0;
}

Dialog::~Dialog()
{
	std::vector<Entry *>::iterator itp;

	for(itp=entries.begin();itp!=entries.end();++itp)
		(*itp)->del();
}

Dialog &Dialog::operator=(const Dialog &src)
{
	std::vector<Entry *>::iterator itp;
	std::vector<StartEntry> startlist;
	std::vector<Entry *> pcee, pcer;
	uint32 idxe=0, idxr=0;

	// Delete existing entries
	for(itp=entries.begin();itp!=entries.end();++itp)
		(*itp)->del();

	preventzoom = src.preventzoom;
	delayentry = src.delayentry;
	delayreply = src.delayreply;
	wordcount = src.wordcount;
	endconv = src.endconv;
	endconvabort = src.endconvabort;

	// Index src's entries
	for(uint32 i=0;i<src.entries.size();i++)
	{
		src.entries[i]->pc = false;
		indexEntries(src.entries[i], idxe, idxr);
	}
	// Copy src's entries into seperate vectors
	for(uint32 i=0;i<src.entries.size();i++)
		copyFlat(src.entries[i], pcee, pcer);
	// Generate startlist
	startlist.resize(src.entries.size());
	for(uint32 i=0;i<src.entries.size();i++)
	{
		startlist[i].index = src.entries[i]->id;
		startlist[i].active = src.active[i];
	}
	// "Tree-ize"
	resolveEntries(startlist, pcee, pcer);

	return *this;
}

Dialog::Dialog(const Dialog &src)
{
	*this = src;
}

void Dialog::indexEntries(Entry *e, uint32 &idxe, uint32 &idxr)
{
	std::vector<Dialog::Entry::Reply>::iterator itr;

	e->id = idxe++;
	for(itr=e->replies.begin();itr!=e->replies.end();++itr)
		if(!itr->link && itr->child)
		{
			itr->child->pc = !e->pc;
			indexEntries(itr->child, idxr, idxe);
		}
}

void Dialog::copyFlat(Entry *e, std::vector<Entry *> &pe, std::vector<Entry *> &pr)
{
	uint32 n;

	pe.resize((n=pe.size())+1);
	pe[n] = new Entry(*e);

	Entry &ne = *pe[n];
	ne.parents.clear();
	ne.repliesids.resize(ne.replies.size());
	for(uint32 i=0;i<ne.replies.size();i++)
	{
		if(!ne.replies[i].child) continue;
		ne.repliesids[i] = ne.replies[i].child->id;
		if(!ne.replies[i].link) copyFlat(ne.replies[i].child, pr, pe);
	}
}

void Dialog::selectFlat(Entry *e, std::vector<Entry *> &pe, std::vector<Entry *> &pr)
{
	std::vector<Entry::Reply>::iterator itr;

	pe.push_back(e);
	for(itr=e->replies.begin();itr!=e->replies.end();++itr)
		if(!itr->link && itr->child) selectFlat(itr->child, pr, pe);
}

int Dialog::load(std::string fname)
{
	GFFFile gff;
	if(gff.open(fname)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Dialog::load(uint8 *mem, uint32 size)
{
	GFFFile gff;
	if(gff.open(mem, size)) return ERR(gff);
	if(gff.init()) return ERR(gff);
	if(load(gff, gff.topstruct)) return errcode;
	return errcode = 0;
}

int Dialog::load(MemSeg &memseg)
{
	return load(memseg.mem, memseg.size);
}

int Dialog::load(GFFFile &gff, GFFFile::Struct &top)
{
	if(gff.type != NWN_FILE_DLG) return errcode = 11;
	std::string tlabelsf[] =
		{"EntryList", "ReplyList", "StartingList"};
	std::string tlabels[] =
		{"PreventZoomIn", "DelayEntry", "DelayReply", "NumWords", "EndConverAbort",
		 "EndConversation"};
	std::vector<uint32> n;
	std::vector<StartEntry> startlist;
	std::vector<Entry *> pcee, pcer;
	std::vector<Entry *>::iterator itp;

	n.resize((sizeof(tlabels)/sizeof(std::string)));
	for(uint32 i=0;i<n.size();i++)
	{
		aderrstr = tlabels[i];
		if((n[i] = gff.getIdxByLabel(tlabels[i], top)) == ERROR)
			return ERR(gff);
	}
	aderrstr.clear();
	if(n[0] != NOTFOUND) preventzoom = (bool) top.values[n[0]];
	if(n[1] != NOTFOUND) delayentry = top.values[n[1]];
	if(n[2] != NOTFOUND) delayreply = top.values[n[2]];
	if(n[3] != NOTFOUND) wordcount = top.values[n[3]];
	if(n[4] != NOTFOUND)
		if(gff.getResRef(n[4], top, endconv)) return ERR(gff);
	if(n[5] != NOTFOUND)
		if(gff.getResRef(n[5], top, endconvabort)) return ERR(gff);

	n.resize((sizeof(tlabelsf)/sizeof(std::string)));
	for(uint32 i=0;i<n.size();i++)
	{
		aderrstr = tlabelsf[i];
		switch(n[i] = gff.getIdxByLabel(tlabelsf[i], top))
		{
			case ERROR: return ERR(gff);
			case NOTFOUND: return errcode = 20;
		}
	}
	aderrstr.clear();
	if(getEntries(gff, n[0], pcee, top)) return errcode;
	if(getEntries(gff, n[1], pcer, top)) return errcode;
	if(getStartList(gff, n[2], startlist, top)) return errcode;
	resolveEntries(startlist, pcee, pcer);
	// If the file was somehow broken, indices could be missing
	for(itp=entries.begin();itp!=entries.end();++itp) (*itp)->tidy();
	if(!wordcount) countWords();

	return errcode = 0;
}

int Dialog::getEntries(GFFFile &gff, uint32 eln, std::vector<Entry *> &entries,
		GFFFile::Struct &top)
{
	std::string labels[] =
		{"Animation", "Delay", "QuestEntry", "Script", "Sound", "Comment",
		 "Speaker", "Quest", "Text", "EntriesList", "RepliesList"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(eln, top, scts)) return ERR(gff);
	entries.resize(scts.size());
	n.resize((sizeof(labels)/sizeof(std::string)));
	for(uint32 i=0;i<scts.size();i++)
	{
		uint32 m;
		for(uint32 j=0;j<n.size();j++)
		{
			aderrstr = labels[j];
			if((n[j] = gff.getIdxByLabel(labels[j], scts[i])) == ERROR)
				return ERR(gff);
		}
		aderrstr.clear();
		entries[i] = new Entry;
		entries[i]->id = scts[i].id;
		if(n[0] != NOTFOUND) entries[i]->anim = scts[i].values[n[0]];
		if(n[1] != NOTFOUND) entries[i]->delay = scts[i].values[n[1]];
		if(n[2] != NOTFOUND) entries[i]->questentry = scts[i].values[n[2]];
		if(n[3] != NOTFOUND)
			if(gff.getResRef(n[3], scts[i], entries[i]->script)) return ERR(gff);
		if(n[4] != NOTFOUND)
			if(gff.getResRef(n[4], scts[i], entries[i]->sound)) return ERR(gff);
		if(n[5] != NOTFOUND)
			if(gff.getExoString(n[5], scts[i], entries[i]->comment)) return ERR(gff);
		if(n[6] != NOTFOUND)
			if(gff.getExoString(n[6], scts[i], entries[i]->speaker)) return ERR(gff);
		if(n[7] != NOTFOUND)
			if(gff.getExoString(n[7], scts[i], entries[i]->quest)) return ERR(gff);
		if(n[8] != NOTFOUND)
			if(gff.getExoLocString(n[8], scts[i], entries[i]->text)) return ERR(gff);
		if(n[9] != NOTFOUND)
		{
			if(getReplies(gff, scts[i], n[9], *entries[i])) return errcode;
		}
		else if(n[10] != NOTFOUND)
			if(getReplies(gff, scts[i], n[10], *entries[i])) return errcode;
	}
	return errcode = 0;
}

int Dialog::getReplies(GFFFile &gff, GFFFile::Struct s, uint32 rln,
		Entry &entry)
{
	std::string labels[] = {"Index", "IsChild", "Active", "LinkComment"};
	std::vector<uint32> n;
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(rln, s, scts)) return ERR(gff);
	entry.replies.resize(scts.size());
	entry.repliesids.resize(scts.size());
	n.resize((sizeof(labels)/sizeof(std::string)));
	for(uint32 i=0;i<scts.size();i++)
	{
		entry.replies[i].child = NULL;
		entry.replies[i].link = false;
		for(uint32 j=0;j<n.size();j++)
		{
			aderrstr = labels[j];
			if((n[j] = gff.getIdxByLabel(labels[j], scts[i])) == ERROR)
				return ERR(gff);
		}
		aderrstr.clear();
		if(n[0] != NOTFOUND) entry.repliesids[i] = scts[i].values[n[0]];
		if(n[1] != NOTFOUND) entry.replies[i].link = (bool) scts[i].values[n[1]];
		if(n[2] != NOTFOUND)
			if(gff.getResRef(n[2], scts[i], entry.replies[i].active))
				return ERR(gff);
		if(n[3] != NOTFOUND)
			if(gff.getExoString(n[3], scts[i], entry.replies[i].linkcomment))
				return ERR(gff);
	}
	return errcode = 0;
}

int Dialog::getStartList(GFFFile &gff, uint32 sln,
		std::vector<StartEntry> &startlist, GFFFile::Struct &top)
{
	std::vector<GFFFile::Struct> scts;

	if(gff.getList(sln, top, scts)) return ERR(gff);
	startlist.resize(scts.size());
	for(uint32 i=0;i<scts.size();i++)
	{
		aderrstr = "Index";
		uint32 n = gff.getIdxByLabel(aderrstr, scts[i]);
		if((n == ERROR) || (n == NOTFOUND)) return errcode = 20;
		startlist[i].index = scts[i].values[n];
		aderrstr = "Active";
		gff.getResRef(aderrstr, scts[i], startlist[i].active);
		aderrstr.clear();
	}
	return errcode = 0;
}

void Dialog::resolveEntries(std::vector<StartEntry> &sl, std::vector<Entry *> &pe,
		std::vector<Entry *> &pr)
{
	std::vector<StartEntry>::iterator its;
	std::vector<Entry *>::iterator ite;

	active.clear();
	entries.resize(sl.size());
	for(its=sl.begin(),ite=entries.begin();its!=sl.end();++its,++ite)
	{
		resolveEntries(pe, pr, &(*ite), NULL, its->index, false, false);
		active.push_back(its->active);
	}
}

void Dialog::resolveEntries(std::vector<Entry *> &pe, std::vector<Entry *> &pr,
		Entry **entry, Entry *parent, uint32 idx, bool link, bool pc)
{
	std::vector<Entry *>::iterator itp;
	std::vector<Entry::Reply>::iterator itr;
	std::vector<uint32>::iterator iti;

	for(itp=pe.begin();itp!=pe.end();++itp)
	{
		if((*itp)->id == idx)
		{
			*entry = *itp;
			(*entry)->pc = pc;
			(*entry)->parents.push_back(parent);
			if(!link)
			{
				itr = (*itp)->replies.begin();
				iti = (*itp)->repliesids.begin();
				while((itr != (*itp)->replies.end()) && (iti != (*itp)->repliesids.end()))
				{
					resolveEntries(pr, pe, &(itr->child), *itp, *iti, itr->link, !pc);
					++itr;
					++iti;
				}
				(*itp)->repliesids.clear();
			}
		}
	}
}

void Dialog::add(Entry *e, std::string active)
{
	entries.push_back(e);
	this->active.push_back(active);
	e->pc = false;
}

void Dialog::del(Entry *e)
{
	std::vector<Entry *>::iterator itp;

	// If e is in the root list, take it out
	itp = entries.begin();
	while(itp != entries.end())
		if(*itp == e) entries.erase(itp);
		else ++itp;

	e->del();
	for(itp=entries.begin();itp!=entries.end();++itp) (*itp)->tidy();
}

int Dialog::save(std::string fname)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite(fname)) return ERR(gff);
	gff.type = NWN_FILE_DLG;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite()) return ERR(gff);
	return errcode = 0;
}

int Dialog::save(uint8 **mem, uint32 *size)
{
	GFFFile gff;
	uint32 top;

	if(gff.beginWrite()) return ERR(gff);
	gff.type = NWN_FILE_DLG;
	if(gff.writeStruct(top, "", 0)) return ERR(gff);
	if(gff.writeSetId(top, 0xFFFFFFFF)) return ERR(gff);
	if(save(gff, top)) return errcode;
	if(gff.endwrite(mem, size)) return ERR(gff);
	return errcode = 0;
}

int Dialog::save(GFFFile &gff, uint32 top)
{
	std::vector<Entry *>::iterator itp;
	uint32 list, sct, idxe=0, idxr=0;
	std::vector<Entry *> pcee, pcer;

	active.resize(entries.size());
	if(gff.writeVar(top, NWN_VAR_INT8, "PreventZoomIn", preventzoom))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "DelayEntry", delayentry))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "DelayReply", delayreply))
		return ERR(gff);
	if(gff.writeVar(top, NWN_VAR_UINT32, "NumWords", wordcount))
		return ERR(gff);
	if(gff.writeResRef(top, "EndConversation", endconv)) return ERR(gff);
	if(gff.writeResRef(top, "EndConverAbort", endconvabort)) return ERR(gff);

	for(itp=entries.begin();itp!=entries.end();++itp)
	{
		(*itp)->pc = false;
		indexEntries(*itp, idxe, idxr);
	}
	for(itp=entries.begin();itp!=entries.end();++itp)
		selectFlat(*itp, pcee, pcer);

	if(gff.writeList(list, top, "EntryList")) return ERR(gff);
	for(itp=pcee.begin();itp!=pcee.end();++itp)
		if(writeEntry(gff, list, **itp)) return errcode;
	if(gff.writeList(list, top, "ReplyList")) return ERR(gff);
	for(itp=pcer.begin();itp!=pcer.end();++itp)
		if(writeEntry(gff, list, **itp)) return errcode;

	if(gff.writeList(list, top, "StartingList")) return ERR(gff);
	for(uint32 i=0;i<entries.size();i++)
	{
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, i)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT32, "Index", entries[i]->id))
			return ERR(gff);
		if(gff.writeResRef(sct, "Active", active[i])) return errcode = 0;
	}
	return errcode = 0;
}

int Dialog::writeEntry(GFFFile &gff, uint32 list, Entry &e)
{
	uint32 sct;

	if(gff.writeListStruct(sct, list)) return ERR(gff);
	if(gff.writeSetId(sct, e.id)) return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_UINT32, "Delay", e.delay)) return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_UINT32, "Animation", e.anim)) return ERR(gff);
	if(gff.writeVar(sct, NWN_VAR_INT8, "AnimLoop", 1)) return ERR(gff);
	if(gff.writeResRef(sct, "Script", e.script)) return ERR(gff);
	if(gff.writeResRef(sct, "Sound", e.sound)) return ERR(gff);
	if(!e.pc && gff.writeExoString(sct, "Speaker", e.speaker)) return ERR(gff);
	if(gff.writeExoString(sct, "Comment", e.comment)) return ERR(gff);
	if(gff.writeExoString(sct, "Quest", e.quest)) return ERR(gff);
	if(!e.quest.empty())
		if(gff.writeVar(sct, NWN_VAR_UINT32, "QuestEntry", e.questentry))
			return ERR(gff);
	if(gff.writeExoLocString(sct, "Text", e.text)) return ERR(gff);
	if(e.pc)
	{
		if(gff.writeList(list, sct, "EntriesList")) return ERR(gff);
	}
	else if(gff.writeList(list, sct, "RepliesList")) return ERR(gff);
	for(uint32 i=0;i<e.replies.size();i++)
	{
		if(!e.replies[i].child) continue;
		if(gff.writeListStruct(sct, list)) return ERR(gff);
		if(gff.writeSetId(sct, i)) return ERR(gff);
		if(gff.writeResRef(sct, "Active", e.replies[i].active)) return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_INT8, "IsChild", e.replies[i].link))
			return ERR(gff);
		if(e.replies[i].link)
			if(gff.writeExoString(sct, "LinkComment", e.replies[i].linkcomment))
				return ERR(gff);
		if(gff.writeVar(sct, NWN_VAR_UINT32, "Index", e.replies[i].child->id))
			return ERR(gff);
	}

	return errcode = 0;
}

uint32 Dialog::countWords(void)
{
	std::vector<Entry *>::iterator itp;
	uint32 n=0;

	for(itp=entries.begin();itp!=entries.end();++itp)
		if(*itp) n += (*itp)->countWords();
	return wordcount = n;
}

// vim:tabstop=2
