#include "headers.p4"

control FlowFilterUpdate (
        in ff_index_t index,
        in ff_data_t update,
	in count_t place,
        out ff_data_t value_1,
        out ff_data_t value_2)
{
    Register<ff_data_t, reg_index_t>(FLOW_FILTER_ENTRIES, 0) flow_filter_1;
    Register<ff_data_t, reg_index_t>(FLOW_FILTER_ENTRIES, 0) flow_filter_2;

    RegisterAction<ff_data_t, reg_index_t, ff_data_t>(flow_filter_1) update_flow_filter_1 = {
        void apply(inout ff_data_t register_data, out ff_data_t result) {
            if (register_data == 0 || register_data > update) {
                register_data = update;
            }
            result = register_data;
        }
    };

    RegisterAction<ff_data_t, reg_index_t, ff_data_t>(flow_filter_2) update_flow_filter_2 = {
        void apply(inout ff_data_t register_data, out ff_data_t result) {
            if (register_data == 0 || register_data > update) {
                register_data = update;
            }
            result = register_data;
        }
    };

    RegisterAction<ff_data_t, reg_index_t, ff_data_t>(flow_filter_1) read_flow_filter_1 = {
        void apply(inout ff_data_t register_data, out ff_data_t result) {
            result = register_data;
        }
    };

    RegisterAction<ff_data_t, reg_index_t, ff_data_t>(flow_filter_2) read_flow_filter_2 = {
        void apply(inout ff_data_t register_data, out ff_data_t result) {
            result = register_data;
        }
    };

    apply {
	value_1 = 0;
	value_2 = 0;
        if (update == 0) {
            value_1 = read_flow_filter_1.execute(index[15:0]);
            value_2 = read_flow_filter_2.execute(index[31:16]);
        } else if (place != 0){
            value_1 = update_flow_filter_1.execute(index[15:0]);
            value_2 = update_flow_filter_2.execute(index[31:16]);
        }
    }
}

control FlowCountUpdate(in rc_index_t index,
                        in bool original,
                        out count_t value)
{
    Register<count_t, rc_index_t>(RECORDER_ENTRIES, 0) fc_table;
    RegisterAction<count_t, rc_index_t, count_t>(fc_table) fc_orig_action = {
        void apply(inout count_t register_data, out count_t result) {
            register_data = register_data + 1;
            result = register_data;
        }
    };

    RegisterAction<count_t, rc_index_t, count_t>(fc_table) fc_sub_action = {
        void apply(inout count_t register_data, out count_t result) {
            result = 0;
            if (register_data == 0) {
                register_data = register_data + 1;
                result = register_data;
            }
        }
    };
    apply {
        if (original) {
            value = fc_orig_action.execute(index);
        } else {
            value = fc_sub_action.execute(index);
        }
    }
}

control StepWidth(in count_t fcnt,
                  out rc_index_t distance) 
{
    MathUnit<rc_index_t>(MathOp_t.MUL, 2) left_shift;

    Register<rc_index_t, _>(1) step_width;
    RegisterAction<rc_index_t, _, rc_index_t>(step_width) cal_step_width = {
        void apply(inout rc_index_t register_data, out rc_index_t result) {
            result = 0;
            if(fcnt > 1) {
                register_data = left_shift.execute(fcnt) % RECORDER_ENTRIES;
                result = register_data;
            }
            
        }
    };

    apply {
        distance = cal_step_width.execute(0); 
    }
}

control RecorderIndex(in rc_index_t index,
                      out rc_index_t mod_index) 
{
    Register<rc_index_t, _>(1) rc_index;
    RegisterAction<rc_index_t, _, rc_index_t>(rc_index) mod_rc_index = { 
        void apply(inout rc_index_t register_data, out rc_index_t result) {
            register_data = index % RECORDER_ENTRIES;
            result = register_data;
        }
    };

    apply {
        mod_index = mod_rc_index.execute(0);
    }
}
