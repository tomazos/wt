create table package_attributes (
  component text,
  package text,
  key text,
  lineno int4,
  value text
);

create table files (
  component text,
  package text,
  relpath text,
  abspath text,
  size int8
);

create table packages (
  id text,
  component text
);

create or replace function category(p text) returns text as $$
begin
  if substr(p,1,3) = 'lib' then
    return substr(p,1,4);
  else
    return substr(p,1,1);
  end if;
end
$$ language plpgsql;

CREATE OR REPLACE VIEW exts as
select 

create or replace view exts as
select substring(relpath from '\.([^\.]*)$') as ext from files;

create or replace view cpackages as
select package, sum(1) as nfiles, sum(size) as total_size from files where cext group by package;

create table ext_counts as
select ext, sum(1) as count from exts group by ext;

copy (select ext, count from ext_counts order by count desc) to '/home/zos/inbox/ext_counts';

'h', 'c', 'cpp', 'cc', 'hpp', 'cxx', 'hxx', 'C'

create table ngrams (
  ngram bytea,
  cext boolean,
  numerator int8,
  denominator int8
);

select encode(ngram, 'escape'), numerator, denominator from ngrams where cext order by numerator desc;
