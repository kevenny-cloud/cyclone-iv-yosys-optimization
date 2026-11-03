// Included inside LutWindowWorker. Conditional cofactor column classes allow
// exactly one original variable to be used by both LUTs. No don't-care rows.
bool synthesize_shared(const std::vector<int>&leaves,Truth table,int required_depth,
                       int dead_count,Plan&plan) {
    int n=GetSize(leaves);
    if(n<5||n>6||dead_count<3)return false;
    for(int subset=1;subset<(1<<n);subset++) {
        budget.charge();int size=__builtin_popcount(unsigned(subset));
        if(size<2||size>4||n-size+2>4)continue;
        std::vector<int> bound,free;
        for(int j=0;j<n;j++)((subset&(1<<j))?bound:free).push_back(j);
        int low_depth=1;
        for(int j:bound)low_depth=std::max(low_depth,depth(leaves[j])+1);
        int high_depth=low_depth+1;
        for(int j:free)high_depth=std::max(high_depth,depth(leaves[j])+1);
        if(high_depth>required_depth)continue;
        for(int shared=0;shared<size;shared++) {
            int c0[2]={-1,-1},c1[2]={-1,-1};uint16_t first=0,second=0;bool valid=true;
            for(int b=0;b<(1<<size)&&valid;b++) {
                int phase=(b>>shared)&1,column=0;
                for(int f=0;f<(1<<GetSize(free));f++) {
                    budget.charge();column|=int(table[expand_address(b,bound)|expand_address(f,free)])<<f;
                }
                if(c0[phase]<0)c0[phase]=column;
                if(column!=c0[phase]) {
                    if(c1[phase]<0)c1[phase]=column;
                    if(column!=c1[phase])valid=false;else first|=1<<b;
                }
            }
            if(!valid)continue;
            for(int phase=0;phase<2;phase++) {
                if(c1[phase]<0)c1[phase]=c0[phase];
                for(int value=0;value<2;value++)for(int f=0;f<(1<<GetSize(free));f++) {
                    budget.charge();int address=f|(phase<<GetSize(free))|(value<<(GetSize(free)+1));
                    int column=value?c1[phase]:c0[phase];second|=((column>>f)&1)<<address;
                }
            }
            for(int row=0;row<(1<<n);row++) {
                budget.charge(1+n);int b=0,f=0;
                for(int j=0;j<size;j++)b|=((row>>bound[j])&1)<<j;
                for(int j=0;j<GetSize(free);j++)f|=((row>>free[j])&1)<<j;
                int address=f|(((row>>bound[shared])&1)<<GetSize(free))|(((first>>b)&1)<<(GetSize(free)+1));
                if(bool((second>>address)&1)!=table[row])log_error("Shared LUT decomposition truth mismatch.\n");
            }
            plan.leaves=leaves;plan.bound=bound;plan.free=free;plan.shared={bound[shared]};
            plan.first=first;plan.second=second;plan.cost=2;plan.depth=high_depth;return true;
        }
    }
    return false;
}
