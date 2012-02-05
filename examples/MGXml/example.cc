gslib::CObject *CTreeBuilder::event(
	gslib::CString const *tag, gslib::CObject *cargo,
	gslib::CString const *key, gslib::CObject *value)
{
	if (tag->is_either("Genome", "Choose", "Pick", NULL))
	{	/* Handle common things. */
		if (this->is_end_tag(key))
		{	/* Complain if the ID is not set of a non-root attr. */
			/* It seems you can't static_cast<CMGObj>(CObject*). */
			if (value && GSLIB_CAST_DYNAMIC(CMGObj const *,
					cargo)->objid == MGOBJ_ANON)
				GSLIB_THROW("attribute missing an id");
			/* Pass through to handle attr-specific finalization. */
		} else if (key->is("id"))
		{
			GSLIB_CAST_DYNAMIC(CMGObj *, cargo)->objid =
				GSLIB_CAST(gslib::CString const *,
					value)->as_unsigned();
			return cargo;
		} else if (key->is("name"))
		{	/* TODO Just ignore it for now. */
			return cargo;
		}
	}

	if (tag->is("Genome"))
	{	/* Cargo is the prototype solution. */
		if (this->is_start_tag(key))
		{
			GSXML_READER_VERIFY_PARENT(value, GSXML_READER_ROOT,
				"Genome", "Choose");
			return this->mkgenome();
		} else if (this->is_end_tag(key))
		{
			CSol *sol;
			CAttr *attr;

			GSLIB_ASSIGN(sol, cargo);
			if (!sol->attrs->npnodes)
				GSLIB_THROW("no attributes set");

			/* Create an attribute out of the cargo. */
			attr = this->mkga(sol->objid);
			GSLIB_ASSERT(attr->gatator != NULL);
			GSLIB_CAST(CGatatorMG *,
				attr->gatator)->pool.push(sol);
			return attr;
		} else if (key->is("Fitness"))
		{
			GSLIB_CAST(CSol *, cargo)->set_fitness(
				GSLIB_CAST(CFitness *, value)->Retain());
			return cargo;
		} else if (key->is_either("Genome", "Choose", "Pick", NULL))
		{
			GSLIB_CAST(CSol *, cargo)->add_attr(
				GSLIB_CAST(CAttr *, value)->Retain());
			return cargo;
		}
	} else if (tag->is("Choose"))
	{	/* Cargo is the attribute. */
		if (this->is_start_tag(key))
		{
			GSXML_READER_VERIFY_PARENT(value,
				GSXML_READER_ROOT, "Genome", "Choose");
			return new CAttr(MGOBJ_ANON, CAttr::ATTRTYPE_COMPOSIT);
		} else if (this->is_end_tag(key))
		{
			if (!GSLIB_CAST(CAttr const *,
					cargo)->subattrs->npnodes)
				GSLIB_THROW("no attributes set");
			return cargo;
		} else if (key->is_either("Genome", "Choose", "Pick", NULL))
		{
			GSLIB_ASSERT(value != NULL);
			GSLIB_CAST(CAttr *, cargo)->add_attr(
				GSLIB_CAST(CAttr *, value)->Retain());
			return cargo;
		}
	} else if (tag->is("Pick"))
	{	/* Cargo is the attribute. */
		/*
		 * Store the value of <Fitness> in $fitness.
		 * This is possible because <Pick>s cannot nest.
		 * TODO This is not very nice because we'll leak it
		 * in case of an excepion.  The proper fix woule be
		 * replaceing $cargo with a struct of { $attr, $fitness }.
		 */
		static CFitnessMG *fitness;

		if (this->is_start_tag(key))
		{
			GSXML_READER_VERIFY_PARENT(value,
				GSXML_READER_ROOT, "Genome", "Choose");
			return new CAttr(MGOBJ_ANON, CAttr::ATTRTYPE_POOL);
		} else if (this->is_end_tag(key))
		{
			if (!GSLIB_CAST(CAttr const *,
					cargo)->solpool->npnodes)
				GSLIB_THROW("no possible solutions set");
			GSOBJ_ZERO(fitness);
			return cargo;
		} else if (key->is("recipe"))
		{
			this->resolve_recipe(GSLIB_CAST(CAttr *, cargo),
				GSLIB_CAST(gslib::CString const *,
					value)->as_unsigned(), fitness);
			return cargo;
		} else if (key->is("set"))
		{
			this->resolve_set(GSLIB_CAST(CAttr *, cargo),
				GSLIB_CAST(gslib::CString const *,
					value)->as_unsigned(), fitness);
			return cargo;
		} else if (key->is("Fitness"))
		{
			if (fitness != NULL)
				GSLIB_THROW("more then one fitness specified");
			GSOBJ_SET(fitness, value);
			return cargo;
		} else if (key->is("Init"))
		{
			CAttr *attr;
			objid_t objid;

			objid = GSLIB_CAST(gslib::CString const *,
				value)->as_unsigned();
			GSLIB_ASSIGN(attr, cargo);
			attr->set_init_sol(objid);
			return cargo;
		}
	} else if (tag->is_either("Recipe", "Set", "Init", NULL))
	{	/* Cargo is the element text itself. */
		if (this->is_start_tag(key))
		{
			GSXML_READER_VERIFY_PARENT(value, "Pick");
			return NULL;
		} else if (this->is_text(key))
		{	/* The parent (Pick) will extract the integer. */
			GSLIB_ASSERT(!cargo);
			return value->Retain();
		} else if (this->is_end_tag(key))
		{
			if (!cargo)
				GSLIB_THROW("set or recipe ID missing");
			return cargo;
		}
	} else if (tag->is("Fitness"))
	{	/* Cargo is the fitness object. */
		if (this->is_start_tag(key))
		{
			GSXML_READER_VERIFY_PARENT(value, "Genome", "Pick");
			return new CFitnessMG_GB();
		} else if (this->is_end_tag(key))
		{	/* Return the fitness object. */
			return cargo;
		} else if (key->is("Bound"))
		{	/* Bound was added in "Bound". */
			GSLIB_ASSERT(!value);
			return cargo;
		}
	} else if (tag->is("Bound"))
	{
		static objid_t valid;
		static CFitnessMG::bound_st bound;

		/* All element-specific things are static,
		 * so we don't need a cargo. */
		if (this->is_start_tag(key))
		{
			GSXML_READER_VERIFY_PARENT(value, "Fitness");
			valid = MGOBJ_ANON;
			memset(&bound, 0, sizeof(bound));
			return NULL;
		} else if (this->is_end_tag(key))
		{	/* Add the bound to the fitness object. */
			GSLIB_CAST(CFitnessMG *, value)->add_bound(
				valid, &bound);
			return NULL;
		} else if (key->is("valid"))
		{
			valid = GSLIB_CAST(gslib::CString const *,
				value)->as_unsigned();
			return cargo;
		} else if (key->is("min"))
		{
			bound.left = GSLIB_CAST(gslib::CString const *,
				value)->as_float();
			return cargo;
		} else if (key->is("opt"))
		{
			bound.opt = GSLIB_CAST(gslib::CString const *,
				value)->as_float();
			return cargo;
		} else if (key->is("max"))
		{
			bound.right = GSLIB_CAST(gslib::CString const *,
				value)->as_float();
			return cargo;
		}
	} /* if */

	GSLIB_THROW("unexpected or invalid element or attribute");
}
