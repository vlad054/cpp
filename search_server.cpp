#include "search_server.h"
#include "iterator_range.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>

vector<string> SplitIntoWords(const string& line) {
  istringstream words_input(line);
  return {istream_iterator<string>(words_input), istream_iterator<string>()};
}

SearchServer::SearchServer(istream& document_input) {
  UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
  InvertedIndex new_index;

  for (string current_document; getline(document_input, current_document); ) {
    new_index.Add(move(current_document));
  }

  index = move(new_index);
}

void SearchServer::AddQueriesStream(
  istream& query_input, ostream& search_results_output
) {

    vector<size_t> vecid(50000,0);

  for (string current_query; getline(query_input, current_query); ) {
    const auto words = SplitIntoWords(current_query);

    // map<size_t, size_t> docid_count;
    vecid.assign(50000,0);

    for (const auto& word : words) {
    // for (const auto& word : SplitIntoWords(current_query)) {
      for (const size_t docid : index.Lookup(word)) {
        // docid_count[docid]++;
        vecid[docid]++;
      }
    }

    // vector<pair<size_t, size_t>> search_results(
    // vector<pair<size_t, size_t>> search_results(
    //   docid_count.begin(), docid_count.end()
    // );

    // cout << " docid_count" << endl;
    // for_each(docid_count.begin(), docid_count.end(),
    // [](pair<size_t, size_t> t){ cout << t.first << " " << t.second << endl;});

    // cout << " vecid" << endl;
    // for(size_t i = 0; i< 50000; i++){
    //     if (vecid[i] > 0) cout << i <<" " << vecid[i] << endl;
    // }


    vector<pair<size_t, size_t>> search_results;

//    for(size_t i=0; i<50000; i++){
      for(size_t i=0; i<50000; i++){
        if (vecid[i] > 0) search_results.push_back({i,vecid[i]});
        // search_results.push_back({i,vecid[i]});

    };


    // sort(
    //   begin(search_results),
    //   end(search_results),
    //   [](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) {
    //     int64_t lhs_docid = lhs.first;
    //     auto lhs_hit_count = lhs.second;
    //     int64_t rhs_docid = rhs.first;
    //     auto rhs_hit_count = rhs.second;
    //     return make_pair(lhs_hit_count, -lhs_docid) > make_pair(rhs_hit_count, -rhs_docid);
    //   }
    // );
    // cout << " before partial_sort" << endl;
    // for(auto f: search_results){
    //      cout << f.first <<" " << f.second << endl;
    // };


    partial_sort(
      begin(search_results),
      begin(search_results) + min<size_t>(5, search_results.size()),
      end(search_results),
      [](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) {
        int64_t lhs_docid = lhs.first;
        auto lhs_hit_count = lhs.second;
        int64_t rhs_docid = rhs.first;
        auto rhs_hit_count = rhs.second;
        return make_pair(lhs_hit_count, -lhs_docid) > make_pair(rhs_hit_count, -rhs_docid);
      }
    );

    // partial_sort(vecid.begin(), vecid.begin()+5, vecid.end());


    // cout << " after partial_sort" << endl;
    // for(auto f: search_results){
    //      cout << f.first <<" " << f.second << endl;
    // };
    size_t ss =  search_results.size();
    search_results_output << current_query << ':';
    // for (auto [docid, hitcount] : Head(search_results, 5)) {
    for (size_t t = 0; (t < 5) && (t < ss) ; t++) {
    //   search_results_output << " {"
    //     << "docid: " << docid << ", "
    //     << "hitcount: " << hitcount << '}';
      search_results_output << " {"
        << "docid: " << search_results[t].first << ", "
        << "hitcount: " << search_results[t].second << '}';
    }
    search_results_output << endl;

    // search_results_output << current_query << ':';
    // for (size_t i=0; i < 5; i++){
    //   search_results_output << " {"
    //     << "docid: " << i << ", "
    //     << "hitcount: " << vecid[i] << '}';
    // }
    // search_results_output << endl;
  }
}

void InvertedIndex::Add(const string& document) {
  docs.push_back(document);

  const size_t docid = docs.size() - 1;
  for (const auto& word : SplitIntoWords(document)) {
    index[word].push_back(docid);
  }
}

list<size_t> InvertedIndex::Lookup(const string& word) const {
  if (auto it = index.find(word); it != index.end()) {
    return it->second;
  } else {
    return {};
  }
}
